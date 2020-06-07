#include "config.h"

#include <stdlib.h>

#include "flags.h"
#include "log.h"

// Fills the fields of a given Config struct with default values.
void initialise_config(struct Config *cfg) {
  cfg->file_path             = DEFAULT_FILE_PATH;
  cfg->new_file_name         = DEFAULT_NEW_FILE_NAME;
  cfg->exclude_headers       = DEFAULT_EXCLUDE_HEADERS;
  cfg->include_remainders    = DEFAULT_INCLUDE_REMAINDERS;
  cfg->delimiter             = DEFAULT_DELIMITER;
  cfg->line_count            = DEFAULT_LINE_COUNT;
  cfg->remove_columns_l      = 0;
  cfg->remove_columns        = NULL;
  cfg->remove_columns_buffer = NULL;
}

size_t count_char(const char *str, char c) {
  size_t count = 0;
  for (const char *p = str; *p; p++) {
    if (*p == c) {
      count++;
    }
  }
  return count;
}

void process_config(struct Config *config) {
  // Big `if` in case we need to add more processing.
  if (config->remove_columns_buffer != NULL) {
    config->remove_columns_l =
        count_char(config->remove_columns_buffer, config->delimiter) + 1;
    LOG("Found %lu columns to be removed.\n", config->remove_columns_l);
    config->remove_columns = malloc(config->remove_columns_l * sizeof(char *));
    char *begin, *end, *header;
    begin = end = config->remove_columns_buffer;
    for (size_t i = 0; i < config->remove_columns_l; i++) {
      while (*end != config->delimiter) {
        end++;
      }
      header = malloc((end - begin + 1) * sizeof(char));
      size_t j;
      for (j = 0; j < end - begin; j++) {
        header[j] = begin[j];
      }
      header[j]                 = '\0';
      config->remove_columns[i] = header;
      begin                     = ++end;
    }
  }
}

// Parses the given command line arguments into the given config struct.
void parse_config(struct Config *cfg, const int argc, char **argv) {
  LOG("Parsing config.\n");
  for (size_t i = 1; i < argc;) {
    i = parse_arg(cfg, argc, argv, i);
  }
  if (!cfg->file_path) {
    fprintf(HELP_OUT, "Expected input file. Use --help to learn more.\n");
    exit(PARSE_ERR);
  }
}

// Prints a string to stderr and adds spaces to the right such that the total
// length is len.
static void print_right_padded(const char *str, size_t len) {
  fprintf(HELP_OUT, "%s", str);
  for (size_t i = 0; i < len - strlen(str); i++) {
    putc(' ', HELP_OUT);
  }
}

// Prints a given Flag struct as required by the --help flag.
static void print_option(struct Flag *f, size_t max) {
  fprintf(HELP_OUT, "\t-%c, --%s ", f->short_id, f->long_id);
  print_right_padded(f->arg, max - strlen(f->long_id));
  fprintf(HELP_OUT, "\t%s\n", f->desc);
}

// Prints useful information about the program as required by the --help flag.
void print_help(void) {
  fprintf(HELP_OUT, "%s\n\n", TITLE);
  fprintf(HELP_OUT, "VERSION: %s\n\n", CSV_SPLIT_VERSION);
  fprintf(HELP_OUT, "USAGE:\n\t%s\n\n", USAGE);
  fprintf(HELP_OUT, "OPTIONS:\n");
  size_t max = max_flag_length();
  for (int i = 0; i < FLAG_COUNT; i++) {
    print_option(FLAGS + i, max);
  }
}
