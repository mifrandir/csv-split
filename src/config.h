#ifndef CONFIG_H
#define CONFIG_H

#include <stdio.h>
#include <string.h>

#define CSV_SPLIT_VERSION "v0.0.2"

struct Config {
  /* Strings */
  const char *file_path;
  const char *new_file_name;
  /* Booleans */
  char exclude_headers;
  char include_remainders;
  /* Other values */
  char delimiter;
  char *remove_columns_buffer;
  size_t line_count;
  size_t remove_columns_l;
  char **remove_columns;
};

// Function to assign default values to cfg fields.
void initialise_config(struct Config *cfg);

// Function to parse given command line arguments to cfg fields.
void parse_config(struct Config *cfg, const int argc, char **argv);

void process_config(struct Config *config);

void print_help(void);

#endif