#ifndef FLAGS_H
#define FLAGS_H

#include <stdio.h>

#include "config.h"

// Represents an input option for the CLI.
//
// Has
// - SHORT: a single character flag
// - LONG: a verbose string flag
// - ARG: a string containing the arguments required as printed by --help
// - DESC: a description of what this does as printed by --help
struct Flag {
  const char short_id;
  const char *long_id;
  const char *arg;
  const char *desc;
};

// Different indices to access the corresponding Flag in the FLAGS array.
enum CLI_FLAG_TYPE {
  NEW_FILE_NAME = 0,
  EXCLUDE_HEADERS,
  LINE_COUNT,
  DELIMITER,
  REMOVE_COLUMNS,
  INCLUDE_REMAINDERS,
  HELP
};

#define DEFAULT_FILE_PATH          NULL
#define DEFAULT_NEW_FILE_NAME      "split"
#define DEFAULT_EXCLUDE_HEADERS    0
#define DEFAULT_LINE_COUNT         1
#define DEFAULT_DELIMITER          ','
#define DEFAULT_INCLUDE_REMAINDERS 0

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
  FLAG('e', "exclude-headers", "", "Exclude headers in new files (default: false)")

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

int parse_arg(struct Config *config, const int argc, const char **argv, size_t at);

size_t max_flag_length();

#endif