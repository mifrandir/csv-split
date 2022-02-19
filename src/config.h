#ifndef CONFIG_H
#define CONFIG_H

#include <stdio.h>
#include <string.h>

#define CSV_SPLIT_VERSION "v0.1.0"

struct Config {
  /* Strings */
  const char *in_file_path;
  const char *out_file_path;
  /* Booleans */
  char do_exclude_headers;
  char do_include_remainders;
  /* Other values */
  char delim;
  char *remove_columns_buffer;
  size_t num_lines;
  size_t remove_columns_l;
  char **remove_columns;
};

// Function to assign default values to cfg fields.
void initialise_config(struct Config *cfg);

// Function to parse given command line arguments to cfg fields.
void parse_config(struct Config *cfg, const int argc, char **argv);

void process_config(struct Config *cfg);

void print_help(void);

#endif