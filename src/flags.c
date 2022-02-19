#include "flags.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "log.h"

// Returns 1 iff the arguments first character is a '-'
static int is_flag(const char *arg) {
  return arg[0] == '-';
}

// Returns 1 iff the arguments second character is a '-'
static int is_long_flag(const char *arg) {
  return arg[1] == '-';
}

// Returns 1 iff the argument only contains numerical digits.
static char is_natural(const char *arg) {
  size_t i = 0;
  while (arg[i]) {
    if (arg[i] < 48 || arg[i] > 57) {
      return 0;
    }
    i++;
  }
  return 1;
}

// Given a path to a file, the columns the user wants to exclude are parsed and
// saved in the config struct.
static int parse_remove_columns(struct Config *cfg, char *arg) {
  if (cfg->remove_columns_buffer != NULL) {
    ERR_LOG("Found two different values for --remove-columns flag.\n");
    return 1;
  }
  cfg->remove_columns_buffer = arg;
  return 0;
}

// Given an index of the argument array and a flag type, the flag and
// potentially its argument are handled.
//
// Returns the index of the argument array where the next argument will be
// found.
//
// Exits if a PARSE_ERR is encountered.
static size_t parse_flag_by_index(
    struct Config *cfg,
    const int argc,
    char **argv,
    size_t *at,
    size_t flag) {
  char *arg;
  register size_t arg_at = *at + 1;
  switch (flag) {
    case NEW_FILE_NAME:
      if (arg_at == argc) {  // Any string will be treated as a file name, as
        // long as it's there.
        ERR_LOG("Expected file name. Use --help to learn more.\n");
        exit(PARSE_ERR);
      }
      arg                = argv[arg_at];
      cfg->out_file_path = arg;
      return *at += 2;  // Read extra argument.
    case EXCLUDE_HEADERS: cfg->do_exclude_headers = 1; return *at += 1;
    case LINE_COUNT:
      if (*at + 1 == argc) {
        ERR_LOG("Expected line count. Use --help to learn more.\n");
        exit(PARSE_ERR);
      }
      if (!is_natural(argv[arg_at])) {
        ERR_LOG("Expected valid positive integer as line count.\n");
      }
      cfg->num_lines = atoi(argv[arg_at]);
      if (cfg->num_lines <= 0) {
        ERR_LOG("Line count needs to be greater than zero.\n");
        exit(PARSE_ERR);
      }
      LOG("Found line count: %lu\n", cfg->num_lines);
      return *at += 2;  // Read extra argument.
    case DELIMITER:
      if (arg_at == argc ||
          strlen(argv[arg_at]) > 1) {  // Expected single character.
        ERR_LOG("Expected delimiter. Use --help to learn more.\n");
        exit(PARSE_ERR);
      }
      cfg->delim = argv[arg_at][0];
      return *at += 2;
    case REMOVE_COLUMNS:
      if (*at + 1 == argc) {  // Any string will be treated as a file name, as
        // lon as it's there.
        ERR_LOG("Expected file name. Use --help to learn more.\n");
        exit(PARSE_ERR);
      }
      arg = argv[arg_at];
      if (parse_remove_columns(cfg, arg)) {
        ERR_LOG("Failed to parse columns to exclude. (file: %s)\n", arg);
        exit(PARSE_ERR);
      }
      return *at += 2;
    case INCLUDE_REMAINDERS: cfg->do_include_remainders = 1; return (*at)++;
    case HELP: print_help(); exit(0);
    default: exit(PARSE_ERR);
  }
}

// A given command line argument is parsed under the assumption that it is a
// short flag. (e.g. -h)
//
// Returns the index of the argument array where the next argument will be
// found.
//
// Exits if a PARSE_ERR is encountered.
static int parse_short_flag(
    struct Config *cfg,
    const int argc,
    char **argv,
    size_t *at) {
  size_t subflag = 1;  // First character is -, so first flag char is at 1.
  size_t start   = *at;
  size_t temp_at;
  char found;
  do {
    found = 0;
    for (struct Flag *f = FLAGS; f < FLAGS + FLAG_COUNT; f++) {
      if (argv[start][subflag] == f->short_id) {
        temp_at = *at;
        LOG("Found short flag for %s\n", f->long_id);
        parse_flag_by_index(cfg, argc, argv, &temp_at, f - FLAGS);
        if (temp_at >
            start +
                1) {  // Boolean flags are not allowed after flags
                      // that require a value. (e.g. `-ne <FILE>` is illegal)
          LOG("Last flag was non-boolean.\n");
          return *at = temp_at;
        }
        found = 1;
        break;
      }
    }
    if (!found) {
      ERR_LOG("Unknown option -%c\n", argv[start][1]);
      exit(PARSE_ERR);
    }
  } while (argv[start][++subflag]);  // Checking for \0 in flag string; intial
                                     // value required to be non \0.
  return *at += 1;
}

// A given command line argument is parsed under the assumption that it is a
// long flag. (e.g. --help)
//
// Returns the index of the argument array where the next argument will be
// found.
//
// Exits if a PARSE_ERR is encountered.
static size_t parse_long_flag(
    struct Config *cfg,
    const int argc,
    char **argv,
    size_t *at) {
  for (struct Flag *f = FLAGS; f < FLAGS + FLAG_COUNT; f++) {
    if (!strcmp(argv[*at] + 2, f->long_id)) {
      return parse_flag_by_index(cfg, argc, argv, at, f - FLAGS);
    }
  }
  ERR_LOG("Unknown option --%s\n", argv[*at]);
  exit(PARSE_ERR);
}

// Given a Flag, this function returns the length this flag will occupy in the
// help output just by its long style flag and the corresponding argument(s)
static size_t flag_length(struct Flag *f) {
  return 3 + strlen(f->long_id) + strlen(f->arg);
}

// Finds the maximum length that any flag requires for printing its long-id and
// arg.
size_t max_flag_length() {
  size_t max = flag_length(FLAGS);
  size_t cur;
  for (struct Flag *p = FLAGS + 1; p != FLAGS + FLAG_COUNT; p++) {
    if ((cur = flag_length(p)) > max) {
      max = cur;
    }
  }
  return max;
}

// Parses the next argument in the argument array.
//
// Returns the index of the argument array where the next argument will be
// found.
//
// Exits if a PARSE_ERR is encountered.
size_t parse_arg(struct Config *cfg, const int argc, char **argv, size_t *at) {
  LOG("Parsing argument at %lu\n", *at);
  if (!is_flag(argv[*at])) {
    LOG("Found file path\n");
    cfg->in_file_path = argv[*at];
    return (*at)++;
  }
  if (!is_long_flag(argv[*at])) {
    LOG("Found short style flag\n");
    return parse_short_flag(cfg, argc, argv, at);
  }
  LOG("Found long style flag\n");
  return parse_long_flag(cfg, argc, argv, at);
}
