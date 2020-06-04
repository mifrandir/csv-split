#include "split.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "config.h"
#include "log.h"

// Replaces every occurence of old in str with new and returns the
// number of replacements.
static size_t strsub(char *str, const char old, const char new) {
  size_t count = 0;
  for (char *p = str; *p != '\0'; p++) {
    if (*p == old) {
      *p = new;
      count++;
    }
  }
  return count;
}

// Replaces the very last '\n' in a given string of a given length with a '\n'
// and returns the new length of the string. Even though the result of strlen
// changes, no memory is allocated or deallocated.
static size_t remove_linebreak(char *str, size_t len) {
  for (char *p = str + len; p >= str; p--) {
    if (*p == '\n') {
      *p = '\0';
      return p - str;
    }
  }
  return len;
}

#define WORKING_DELIM '\0'

static void load_headers(
    const struct Config *cfg, FILE *f, size_t *len, char ***headers) {
  LOG("Getting file headers\n");
  if (f == NULL) {
    ERR_LOG("Received NULL as file.\n");
    exit(EXIT_FAILURE);
  }
  // Reading the line
  size_t line_len = 0;
  char *line      = NULL;
  getline(&line, &line_len, f);
  if (!line) {
    exit(EXIT_FAILURE);
  }
  line_len = remove_linebreak(line, line_len);
  // Counting columns and replacing delim with constant WORKING_DELIM
  *len = strsub(line, cfg->delimiter, WORKING_DELIM) +
         1;   // +1 because there is one more field than delimiters.
  // Inserting column headers
  *headers = malloc(*len * sizeof(char *));
  char *begin, *end, *new_header;
  size_t i = 0;
  for (begin = line; begin - line < line_len; begin++) {
    for (end = begin; *end != WORKING_DELIM; end++)
      ;   // The end of every column header is WORKING_DELIM now.
    new_header = calloc(end - begin + 1, sizeof(char));
    strcpy(new_header, begin);
    (*headers)[i++] = new_header;
    begin           = end;
  }
  free(line);
}

size_t file_count = 0;
size_t line_count = 0;

static size_t find_relevant_columns(
    const struct Config *config,
    const size_t input_length,
    char **headers,
    char *include_column) {
  size_t output_length = input_length;
  for (size_t i = 0; i < output_length; i++) {
    include_column[i] = 1;
    for (size_t j = 0; j < config->remove_columns_l; j++) {
      if (!strcmp(headers[i], config->remove_columns[j])) {
        include_column[i] = 0;
        output_length--;
        break;
      }
    }
  }
  return output_length;
}

static void filter_line(
    const struct Config *config,
    const size_t input_length,          // number of columns in input file
    char **input,                       // string values for each input column
    const char *include_column,         // whether the ith column should be written to
    const size_t ouput_length,          // number of columns in output file
                                        // output
    char **output) {                    // buffer for output values
  if (ouput_length == input_length) {   // If there's nothing to filter, we can
                                        // just copy the enitre array.
    memcpy(output, input, input_length * sizeof(char *));
    LOG("Skipped filtering because input and output sizes are identical.\n");
    return;
  }
  size_t i, j;
  for (i = j = 0; j < ouput_length; j++, i++) {
    while (!include_column[i]) {   // Skipping all the columns that have been
                                   // excluded
      i++;
    }
    output[j] = input[i];
  }
}

struct Batch {
  size_t line_count;
  size_t column_count;
  size_t *line_lengths;
  char **lines;
  char ***values;
  char *file_name;
};

// Loads the values from a given line into an array. None of the strings are copied so
// they rely on the line buffer being valid. Additionally, the line buffer is altered to
// handle those references.
static void load_values(
    const struct Config *config,
    char *line,   // A line buffer containing comma separated values WITHOUT the
                  // linebreak
    const size_t input_column_count,
    char **values) {
  size_t actual_column_count;
  remove_linebreak(line, strlen(line));
  if (input_column_count !=
      (actual_column_count = strsub(line, config->delimiter, WORKING_DELIM) + 1)) {
    fprintf(
        stderr,
        "Unexpected number of columns in input on line %lu. (expected: %lu, "
        "found: %lu)\n",
        line_count,
        input_column_count,
        actual_column_count);
    exit(1);
  }
  char *begin, *end, **next_value;
  next_value = values;
  for (begin = line; next_value - values < input_column_count; begin++) {
    for (end = begin; *end != WORKING_DELIM; end++)
      ;
    *(next_value++) = begin;
    begin           = end;
  }
  for (size_t i = 0; i < input_column_count; i++) {
    char *p = values[i];
    while (*p != '\0') {
      if (*(p++) == '\n') {
        LOG("WARNING: Found newline in value\n");
      }
    }
  }
  LOG("Wrote %lu values\n", next_value - values);
}

static void write_line(
    const struct Config *config, FILE *f, const size_t column_count, char **values) {
  fprintf(f, "%s", values[0]);   // Having less than one column is impossible.
  for (size_t i = 1; i < column_count; i++) {
    fprintf(f, "%c%s", config->delimiter, values[i]);
  }
  putc('\n', f);
}

static void write_batch(
    const struct Config *config, char **headers, struct Batch *output) {
  LOG("Writing batch #%lu to file (%s).\n", file_count - 1, output->file_name);
  FILE *f = fopen(output->file_name, "w+");
  if (f == NULL) {
    ERR_LOG(
        "An error occured opening or creating file #%lu (%s).\n",
        file_count - 1,
        output->file_name);
    exit(1);
  }
  if (!config->exclude_headers) {
    LOG("Writing headers.\n");
    write_line(config, f, output->column_count, headers);
  }
  LOG("Writing values.\n");
  for (size_t i = 0; i < output->line_count; i++) {
    write_line(config, f, output->column_count, output->values[i]);
  }
  fclose(f);
}

static void process_batch(
    const struct Config *config,
    FILE *file,
    const size_t input_column_count,
    const char *include_column,
    char **headers,
    struct Batch *output) {   // Buffer that holds all the necessary information
                              // for the current set of lines.
  char **line;
  size_t *len;
  size_t i;
  ssize_t read;
  char *value_buffer[output->column_count];
  sprintf(output->file_name, "%s%lu.csv", config->new_file_name, file_count++);
  for (i = 0; i < config->line_count; i++) {
    line = &output->lines[i];
    len  = &output->line_lengths[i];
    read = getline(line, len, file);
    if (read == -1) {
      if (config->include_remainders) {
        LOG("Writing remainders\n");
        output->line_count = i;
        write_batch(config, headers, output);
      }
      return;
    }
    load_values(config, *line, input_column_count, value_buffer);
    filter_line(
        config,
        input_column_count,
        value_buffer,
        include_column,
        output->column_count,
        output->values[i]);
  }
  output->line_count = i;
  write_batch(config, headers, output);
}

static void initialise_batch(
    const struct Config *config,
    struct Batch *batch,
    const size_t max_lines,
    const size_t cols) {
  if (batch->lines == NULL) {
    // If the buffers have not yet been initialised, we allocate the necessary
    // memory.
    batch->lines        = malloc(max_lines * sizeof(char *));
    batch->values       = malloc(max_lines * sizeof(char **));
    batch->line_lengths = malloc(max_lines * sizeof(size_t));
    batch->file_name    = malloc(strlen(config->new_file_name) + FILE_NAME_SUFFIX_BUFFER);
    for (char ***p = batch->values; p - batch->values < max_lines; p++) {
      *p = malloc(cols * sizeof(char *));
    }
    for (size_t *p = batch->line_lengths; p - batch->line_lengths < max_lines; p++) {
      *p = 0;
    }
  }
  batch->line_count   = 0;
  batch->column_count = cols;
}

void split_csv(const struct Config *cfg) {
  LOG("Splitting file\n");
  if (access(cfg->file_path, F_OK) == -1) {
    fprintf(stderr, "File does not exist. (%s)\n", cfg->file_path);
    exit(EXIT_FAILURE);
  }
  FILE *f = fopen(cfg->file_path, "r");
  // Processing file header.
  // TODO: Include option to process files without headers.
  char **input_headers = NULL;
  size_t input_length  = 0;
  load_headers(cfg, f, &input_length, &input_headers);
  LOG("Filtering file headers\n");
  char include_column[input_length];
  size_t output_length =
      find_relevant_columns(cfg, input_length, input_headers, include_column);
  char *output_headers[output_length];
  filter_line(
      cfg, input_length, input_headers, include_column, output_length, output_headers);
  LOG("Headers parsed: input_cols=%lu, output_cols=%lu\n", input_length, output_length);
  struct Batch output_buffer = {0, 0, NULL, NULL, NULL};
  do {
    initialise_batch(cfg, &output_buffer, cfg->line_count, output_length);
    process_batch(cfg, f, input_length, include_column, output_headers, &output_buffer);
  } while (output_buffer.line_count == cfg->line_count);
}