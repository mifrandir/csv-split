
#include <stdlib.h>

#ifndef CLI_H
#define CLI_H

#define ARG_ERR   -1
#define PARSE_ERR -2

#define DEFAULT_FILE_PATH          NULL
#define DEFAULT_NEW_FILE_NAME      "split"
#define DEFAULT_EXCLUDE_HEADERS    0
#define DEFAULT_LINE_COUNT         1
#define DEFAULT_DELIMITER          ','
#define DEFAULT_INCLUDE_REMAINDERS 0

struct Config {
  /* Strings */
  const char *file_path;
  const char *new_file_name;
  /* Boleans */
  char exclude_headers;
  char include_remainders;
  /* Other values */
  char delimiter;
  size_t line_count;
  size_t remove_columns_l;
  char **remove_columns;
};

struct Flag {
  const char short_id;
  const char *long_id;
  const char *arg;
  const char *desc;
};

enum CLI_FLAG_TYPE {
  NEW_FILE_NAME = 0,
  EXCLUDE_HEADERS,
  LINE_COUNT,
  DELIMITER,
  REMOVE_COLUMNS,
  INCLUDE_REMAINDERS,
  HELP
};

#define FLAG(s, l, a, d) \
  { s, l, a, d }

#define TITLE "Split CSV files by lines"

#define USAGE "csv-split [OPTIONS] <FILE_TO_SPLIT>"

#define FLAG_NEW_FILE_NAME                                                \
  FLAG(                                                                   \
      'n',                                                                \
      "new-file-name",                                                    \
      "<NEW_FILE>",                                                       \
      "Name of the new files. This will be appended with an incremented " \
      "number (default: \"split\")")

#define FLAG_EXCLUDE_HEADERS \
  FLAG(                      \
      'e',                   \
      "exclude-headers",     \
      "",                    \
      "Exclude headers in new files (default: false)")

#define FLAG_LINE_COUNT \
  FLAG('l', "line-count", "<COUNT>", "Number of lines per file (default: 1)")

#define FLAG_DELIMITER \
  FLAG(                \
      'd',             \
      "delimiter",     \
      "<DELIMITER>",   \
      "Character used for column separation (default: ',')")

#define FLAG_REMOVE_COLUMNS                                                \
  FLAG(                                                                    \
      'r',                                                                 \
      "remove-columns",                                                    \
      "<FILE>",                                                            \
      "Specify column names to be removed during processing in specified " \
      "file.")

#define FLAG_INCLUDE_REMAINDERS \
  FLAG(                         \
      'i',                      \
      "include-remainders",     \
      "",                       \
      "Include remainder rows in the split files (default: false).")

#define FLAG_HELP FLAG('h', "help", "", "Display this message")

#define FLAG_COUNT (sizeof FLAGS / sizeof FLAGS[0])
static struct Flag FLAGS[] = {FLAG_NEW_FILE_NAME,
                              FLAG_EXCLUDE_HEADERS,
                              FLAG_LINE_COUNT,
                              FLAG_DELIMITER,
                              FLAG_REMOVE_COLUMNS,
                              FLAG_INCLUDE_REMAINDERS,
                              FLAG_HELP};

// Function to assign default values to cfg fields.
void initialise_config(struct Config *cfg);

// Function to parse given command line arguments to cfg fields.
void parse_config(struct Config *cfg, const int argc, const char **argv);

// Prints helpful information about this program
void print_help(void);

#endif