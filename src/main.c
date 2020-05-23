#define _GNU_SOURCE
#include "main.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(const int argc, const char *argv[]) {
  struct Config cfg;
  initialise_config(&cfg);
  parse_config(&cfg, argc, argv);
  split_csv(&cfg);
}

void load_headers(
    const struct Config *cfg, FILE *f, char ***headers, size_t *len) {
  if (f == NULL) {
    exit(EXIT_FAILURE);
  }
  // Reading the line
  size_t read;
  size_t line_len = 0;
  char *line      = NULL;
  read            = getline(&line, &line_len, f);
  if (!line) {
    exit(EXIT_FAILURE);
  }
  // Counting columns
  *len = 1;
  for (char *p = line; p - line < line_len; p++) {
    if (*p == cfg->delimiter) {
      (*len)++;
      *p = '\0';   // Replacing delimiters with \0 to make life easier.
    }
  }
  // Inserting column headers
  *headers = malloc(*len * sizeof(char *));
  char *begin, *end, *new_header;
  size_t i = 0;
  for (begin = line; begin - line < line_len; begin++) {
    for (end = begin; *end != '\0'; end++)
      ;   // The end of every column header is \0 now.
    new_header = calloc(end - begin + 1, sizeof(char));
    strcpy(new_header, begin);
    (*headers)[i++] = new_header;
  }
  free(line);
}

void split_csv(const struct Config *cfg) {
  if (access(cfg->file_path, F_OK) == -1) {
    fprintf(stderr, "File does not exist. (%s)\n", cfg->file_path);
  }
  FILE *f        = fopen(cfg->file_path, "r");
  char **headers = NULL;
  size_t len     = 0;
  load_headers(cfg, f, &headers, &len);
  fprintf(stderr, "Found %lu columns.\n", len);
}