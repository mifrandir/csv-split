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
  char *p;
  for (p = str; *p != '\0'; p++) {
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
  for (char *p = str + len; p != str; p--) {
    if (*p == '\n') {
      *p = '\0';
      return p - str;
    }
  }
  return len;
}

#define WORKING_DELIM '\0'

// Reads the first line of a file and extracts its headers.
static void load_headers(
    const struct Config *cfg,
    FILE *f,
    size_t *len,
    char ***headers) {
  LOG("Getting file headers\n");
  if (f == NULL) {
    ERR_LOG("Received NULL as file.\n");
    exit(EXIT_FAILURE);
  }
  // Reading the line
  size_t line_buffer_len = 0;
  char *line             = NULL;
  ssize_t line_len       = getline(&line, &line_buffer_len, f);
  if (!line) {
    exit(EXIT_FAILURE);
  }
  line_len = remove_linebreak(line, line_len);
  // Counting columns and replacing delim with constant WORKING_DELIM
  *len = strsub(line, cfg->delim, WORKING_DELIM) +
         1;  // +1 because there is one more field than delimiters.
  // Inserting column headers
  *headers = malloc(*len * sizeof(char *));
  char *begin, *end, *new_header;
  size_t i = 0;
  for (begin = line; begin - line < line_len; begin++) {
    for (end = begin; *end != WORKING_DELIM; end++)
      ;  // The end of every column header is WORKING_DELIM now.
    new_header = calloc(end - begin + 1, sizeof(char));
    strcpy(new_header, begin);
    (*headers)[i++] = new_header;
    begin           = end;
  }
  free(line);
}

// How many files have been written.
size_t num_files = 0;

// Removes the columns specified in the CONFIG from the INCLUDE_COLUMN set.
static size_t find_relevant_columns(
    const struct Config *cfg,
    const size_t input_length,
    char **headers,
    char *include_column) {
  size_t output_length = input_length;
  for (size_t i = 0; i < input_length; i++) {
    include_column[i] = 1;
    for (size_t j = 0; j < cfg->remove_columns_l; j++) {
      if (!strcmp(headers[i], cfg->remove_columns[j])) {
        include_column[i] = 0;
        output_length--;
        break;
      }
    }
  }
  return output_length;
}

// Given a config and a set of input values, this functions generates an output
// array containing only those columns that are not to be excluded by the
// config.
static void filter_line(
    const struct Config *cfg,
    const size_t input_length,   // number of columns in input file
    char **input,                // string values for each input column
    const char *include_column,  // whether the ith column should be written to
    const size_t output_length,  // number of columns in output file
                                 // output
    char **output) {             // buffer for output values
  if (output_length == input_length) {  // If there's nothing to filter, we can
                                        // just copy the enitre array.
    memcpy(output, input, input_length * sizeof(char *));
    LOG("Skipped filtering because input and output sizes are identical.\n");
    return;
  }
  size_t i, j;
  for (i = j = 0; j < output_length; j++, i++) {
    while (i < input_length &&
           !include_column[i]) {  // Skipping all the columns that
                                  // have been excluded
      i++;
    }
    output[j] = input[i];
  }
}

// Holds memory for the current set of lines that is being processed.
struct Batch {
  size_t num_lines;
  size_t num_cols;
  size_t *line_lens;
  char **lines;
  char ***values;
  char *file_name;
};

// Loads the values from a given line into an array. None of the strings are
// copied so they rely on the line buffer being valid. Additionally, the line
// buffer is altered to handle those references.
static void load_values(
    const struct Config *cfg,
    char *line,  // A line buffer containing comma separated values WITHOUT the
                 // linebreak
    const size_t input_column_count,
    const size_t output_column_count,
    char **values) {
  size_t actual_num_cols;
  remove_linebreak(line, strlen(line));
  if (input_column_count !=
      (actual_num_cols =
           strsub(line, cfg->delim, WORKING_DELIM) + 1)) {
    fprintf(
        stderr,
        "Unexpected number of columns in input. (expected: %lu, found: %lu)\n",
        input_column_count,
        actual_num_cols);
    exit(1);
  }
  char *begin, *end, **next_value;
  next_value = values;
  for (begin = line; next_value - values < output_column_count; begin++) {
    for (end = begin; *end != WORKING_DELIM; end++)
      ;
    *(next_value++) = begin;
    begin           = end;
  }
  LOG("Wrote %lu values\n", next_value - values);
}

// Writes a single line to F filled with COLUMN_COUNT fields of VALUES separated
// by CONFIG->DELIMITER.
static void write_line(
    const struct Config *cfg,
    FILE *f,
    const size_t column_count,
    char **values) {
  fprintf(f, "%s", values[0]);  // Having less than one column is impossible.
  for (size_t i = 1; i < column_count; i++) {
    fprintf(f, "%c%s", cfg->delim, values[i]);
  }
  putc('\n', f);
}

// Writes OUTPUT to the correct file using CONFIG and HEADERS for formatting.
static void write_batch(
    const struct Config *cfg,
    char **headers,
    struct Batch *output) {
  LOG("Writing batch #%lu to file (%s).\n", num_files - 1, output->file_name);
  FILE *f = fopen(output->file_name, "w+");
  if (f == NULL) {
    ERR_LOG(
        "An error occured opening or creating file #%lu (%s).\n",
        num_files - 1,
        output->file_name);
    exit(1);
  }
  if (!cfg->do_exclude_headers) {
    LOG("Writing headers.\n");
    write_line(cfg, f, output->num_cols, headers);
  }
  LOG("Writing values.\n");
  for (size_t i = 0; i < output->num_lines; i++) {
    write_line(cfg, f, output->num_cols, output->values[i]);
  }
  fclose(f);
}

// Reads the next set of lines from the input and processes them.
static void process_batch(
    const struct Config *cfg,
    FILE *file,
    const size_t input_column_count,
    const char *include_column,
    char **headers,
    struct Batch *output) {  // Buffer that holds all the necessary information
                             // for the current set of lines.
  LOG("Processing batch.\n");
  char **line_ptr;
  size_t *len_ptr, i;
  ssize_t read;
  char *value_buffer[output->num_cols];
  sprintf(output->file_name, "%s%lu.csv", cfg->out_file_path, num_files++);
  for (i = 0; i < cfg->num_lines; i++) {
    line_ptr          = output->lines + i;
    len_ptr           = output->line_lens + i;
    read          = getline(line_ptr, len_ptr, file);
    if (read == -1) {
      if (cfg->do_include_remainders) {
        LOG("Writing remainders\n");
        break;
      }
      return;
    }
    load_values(
        cfg,
        *line_ptr,
        input_column_count,
        output->num_cols,
        value_buffer);
    filter_line(
        cfg,
        input_column_count,
        value_buffer,
        include_column,
        output->num_cols,
        output->values[i]);
  }
  output->num_lines = i;
  write_batch(cfg, headers, output);
}

// Given a new BATCH this function initialises its values to be ready for use.
static void initialise_batch(
    const struct Config *cfg,
    struct Batch *batch,
    const size_t max_lines,
    const size_t cols) {
  if (batch->lines == NULL) {
    // If the buffers have not yet been initialised, we allocate the necessary
    // memory.
    batch->lines = malloc(max_lines * sizeof(char *));
    LOG("Allocating values\n");
    batch->values       = malloc(max_lines * sizeof(char **));
    batch->line_lens = malloc(max_lines * sizeof(size_t));
    batch->file_name =
        malloc(strlen(cfg->out_file_path) + LEN_FILE_NAME_SUFFIX_BUFFER);
    for (char ***p = batch->values; p - batch->values < max_lines; p++) {
      LOG("Allocating sub_values\n");
      *p = malloc(cols * sizeof(char *));
    }
    for (size_t *q = batch->line_lens; q - batch->line_lens < max_lines;
         q++) {
      *q = LEN_INITIAL_LINE_BUFFER;
    }
    for (char **r = batch->lines; r - batch->lines < max_lines; r++) {
      *r = malloc(LEN_INITIAL_LINE_BUFFER * sizeof(char));
    }
  }
  batch->num_lines   = 0;
  batch->num_cols = cols;
}

// Splits a file by lines and writes them to outputs as specified in CFG.
void split_csv(const struct Config *cfg) {
  LOG("Splitting file\n");
  if (access(cfg->in_file_path, F_OK) == -1) {
    fprintf(stderr, "File does not exist. (%s)\n", cfg->in_file_path);
    exit(EXIT_FAILURE);
  }
  FILE *f = fopen(cfg->in_file_path, "r");
  // Processing file header.
  // TODO: Include option to process files without headers.
  char **input_headers = NULL;
  size_t input_length  = 0;
  load_headers(cfg, f, &input_length, &input_headers);
  LOG("Filtering file headers\n");
  char include_column[input_length];
  size_t output_length =
      find_relevant_columns(cfg, input_length, input_headers, include_column);
  LOG("input_length=%lu, output_length=%lu\n", input_length, output_length);
  char *output_headers[output_length];
  filter_line(
      cfg,
      input_length,
      input_headers,
      include_column,
      output_length,
      output_headers);
  LOG("Headers parsed: input_cols=%lu, output_cols=%lu\n",
      input_length,
      output_length);
  struct Batch output_buffer = {0, 0, NULL, NULL, NULL};
  do {
    initialise_batch(cfg, &output_buffer, cfg->num_lines, output_length);
    process_batch(
        cfg,
        f,
        input_length,
        include_column,
        output_headers,
        &output_buffer);
  } while (output_buffer.num_lines == cfg->num_lines);
}