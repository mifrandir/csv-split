#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
  if (argc != 4) {
    fprintf(stderr, "Expected 3 argumens, found %d\n", argc);
    exit(1);
  }
  int num_rows     = atoi(argv[1]);
  int num_cols     = atoi(argv[2]);
  int len_col      = atoi(argv[3]);
  size_t len_row   = num_cols * (len_col + 1);
  char *row_buffer = malloc(len_row * sizeof(char));
  int i, j;
  for (i = 0; i < len_row - 1; i += len_col + 1) {
    for (j = 0; j < len_col; j++) {
      row_buffer[i + j] = 'A';
    }
    row_buffer[i + j] = ',';
  }
  row_buffer[i-1] = '\0';
  for (i = 0; i < num_rows; i++) {
    printf("%s\n", row_buffer);
  }
}