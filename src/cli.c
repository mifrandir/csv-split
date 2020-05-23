#include "cli.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void initialise_config(struct Config *cfg) {
  cfg->file_path          = DEFAULT_FILE_PATH;
  cfg->new_file_name      = DEFAULT_NEW_FILE_NAME;
  cfg->exclude_headers    = DEFAULT_EXCLUDE_HEADERS;
  cfg->include_remainders = DEFAULT_INCLUDE_REMAINDERS;
  cfg->delimiter          = DEFAULT_DELIMITER;
  cfg->line_count         = DEFAULT_LINE_COUNT;
  cfg->remove_columns_l   = 0;
  cfg->remove_columns     = NULL;
}

// Returns 1 iff the arguments first character is a '-'
int is_flag(const char *arg) {
  return arg[0] == '-';
}

// Returns 1 iff the arguments second character is a '-'
int is_long_flag(const char *arg) {
  return arg[1] == '-';
}

// Returns 1 iff the argument only contains numerical digits.
int is_natural(const char *arg) {
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
int parse_remove_columns(struct Config *cfg, const char *file) {
  fprintf(stderr, "Removing columns has not yet been implemented.\n");
  return 0;
}

// Given an index of the argument array and a flag type, the flag and
// potentially its argument are handled.
//
// Returns the index of the argument array where the next argument will be
// found.
//
// Exits if a PARSE_ERR is encountered.
size_t parse_flag_by_index(
    struct Config *cfg,
    const int argc,
    const char **argv,
    size_t at,
    size_t flag) {
  const char *arg;
  switch (flag) {
    case NEW_FILE_NAME:
      if (at + 1 == argc) {   // Any string will be treated as a file name, as
                              // long as it's there.
        fprintf(stderr, "Expected file name. Use --help to learn more.\n");
        exit(PARSE_ERR);
      }
      arg                = argv[at + 1];
      cfg->new_file_name = arg;
      return at + 2;   // Read extra argument.
    case EXCLUDE_HEADERS: cfg->exclude_headers = 1; return at + 1;
    case LINE_COUNT:
      if (at + 1 == argc) {
        fprintf(stderr, "Expected line count. Use --help to learn more.\n");
        exit(PARSE_ERR);
      }
      if (!is_natural(argv[at + 1])) {
        fprintf(stderr, "Expected valid positive integer as line count.\n");
      }
      cfg->line_count = atoi(arg);
      if (cfg->line_count <= 0) {
        fprintf(stderr, "Line count needs to be greater than zero.\n");
        exit(PARSE_ERR);
      }
      return at + 2;   // Read extra argument.
    case DELIMITER:
      if (at + 1 == argc ||
          strlen(argv[at + 1]) > 1) {   // Expected single character.
        fprintf(stderr, "Expected delimiter. Use --help to learn more.\n");
        exit(PARSE_ERR);
      }
      cfg->delimiter = argv[at + 1][0];
      return at + 2;
    case REMOVE_COLUMNS:
      if (at + 1 == argc) {   // Any string will be treated as a file name, as
                              // lon as it's there.
        fprintf(stderr, "Expected file name. Use --help to learn more.\n");
        exit(PARSE_ERR);
      }
      arg = argv[at + 1];
      if (!parse_remove_columns(cfg, arg)) {
        fprintf(
            stderr, "Failed to parse columns to exclude. (file: %s)\n", arg);
        exit(PARSE_ERR);
      }
      return at + 2;
    case INCLUDE_REMAINDERS: cfg->include_remainders = 1; return at + 1;
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
int parse_short_flag(
    struct Config *cfg, const int argc, const char **argv, size_t at) {
  size_t subflag = 1;   // First character is -, so first flag char is at 1.
  size_t new_at;
  char found;
  do {
    found = 0;
    for (struct Flag *f = FLAGS; f < FLAGS + FLAG_COUNT; f++) {
      if (argv[at][subflag] == f->short_id) {
        new_at = parse_flag_by_index(cfg, argc, argv, at, f - FLAGS);
        if (new_at > at + 1) {   // Boolean flags are not allowed after flags
                                 // that require a value.
          return new_at;
        }
        found = 1;
        break;
      }
    }
    if (!found) {
      fprintf(stderr, "Unknown option -%c\n", argv[at][1]);
      exit(PARSE_ERR);
    }
  } while (argv[at][++subflag]);   // Checking for \0 in flag string; intial
                                   // value required to be non \0.
  return at + 1;
}

// A given command line argument is parsed under the assumption that it is a
// long flag. (e.g. --help)
//
// Returns the index of the argument array where the next argument will be
// found.
//
// Exits if a PARSE_ERR is encountered.
size_t parse_long_flag(
    struct Config *cfg, const int argc, const char **argv, size_t at) {
  for (struct Flag *f = FLAGS; f < FLAGS + FLAG_COUNT; f++) {
    if (!strcmp(argv[at] + 2, f->long_id)) {
      return parse_flag_by_index(cfg, argc, argv, at, f - FLAGS);
    }
  }
  fprintf(stderr, "Unknown option --%s\n", argv[at]);
  exit(PARSE_ERR);
}

// Parses the next argument in the argument array.
//
// Returns the index of the argument array where the next argument will be
// found.
//
// Exits if a PARSE_ERR is encountered.
int parse_arg(
    struct Config *cfg, const int argc, const char **argv, size_t at) {
  if (at < 1 || at >= argc) {
    return argc;
  }
  if (!is_flag(argv[at])) {
    cfg->file_path = argv[at];
  }
  if (!is_long_flag(argv[at])) {
    return parse_short_flag(cfg, argc, argv, at);
  }
  return parse_long_flag(cfg, argc, argv, at);
}

size_t flag_length(struct Flag *f) {
  return 3 + strlen(f->long_id) + strlen(f->arg);
}

// Finds the maximum length that any flag requires for printing its long-id and
// arg.
size_t max_flag_length() {
  size_t max = flag_length(FLAGS);
  size_t cur;
  for (struct Flag *p = FLAGS; p < FLAGS + FLAG_COUNT; p++) {
    if ((cur = flag_length(p)) > max) {
      cur = max;
    }
  }
  return max;
}

#define HELP_OUT stderr

// Prints a string to stderr and adds spaces to the right such that the total
// length is len.
void print_right_padded(const char *str, size_t len) {
  fprintf(HELP_OUT, "%s", str);
  for (size_t i = 0; i < len - strlen(str); i++) {
    putc(' ', HELP_OUT);
  }
}

// Prints a given Flag struct as required by the --help flag.
void print_option(struct Flag *f, size_t max) {
  fprintf(HELP_OUT, "\t-%c, --%s ", f->short_id, f->long_id);
  print_right_padded(f->arg, max - strlen(f->long_id));
  fprintf(HELP_OUT, "\t%s\n", f->desc);
}

// Prints useful information about the program as required by the --help flag.
void print_help(void) {
  fprintf(HELP_OUT, "%s\n\n", TITLE);
  fprintf(HELP_OUT, "USAGE:\n\t%s\n\n", USAGE);
  fprintf(HELP_OUT, "OPTIONS:\n");
  size_t max = max_flag_length();
  for (int i = 0; i < FLAG_COUNT; i++) {
    print_option(FLAGS + i, max);
  }
}

// Parses the given command line arguments into the given config struct.
void parse_config(struct Config *cfg, const int argc, const char **argv) {
  for (size_t i = 1; i < argc;) {
    i = parse_arg(cfg, argc, argv, i);
  }
  if (!cfg->file_path) {
    fprintf(HELP_OUT, "Expected input file. Use --help to learn more.\n");
    exit(PARSE_ERR);
  }
}