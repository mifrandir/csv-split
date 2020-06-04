#include <stdio.h>
#include <stdlib.h>

#include "../log.h"

int main(const int argc, const char **argv) {
  if (argc != 3) {
    ERR_LOG("Expected two arguments to compare.\n");
  }
  FILE *file_a = fopen(argv[1], "r");
  if (file_a == NULL) {
    ERR_LOG("Failure opening %s: file does not exist.\n", argv[1]);
    exit(1);
  }
  FILE *file_b = fopen(argv[2], "r");
  if (file_b == NULL) {
    ERR_LOG("Failure opening %s: file does not exist.\n", argv[2]);
    exit(1);
  }
  int read_a, read_b;
  for (size_t num_chars = 0; !feof(file_a) && !feof(file_b); num_chars++) {
    if (ferror(file_a)) {
      ERR_LOG("Failure reading file %s\n", argv[1]);
      exit(1);
    }
    if (ferror(file_b)) {
      ERR_LOG("Failure reading file %s\n", argv[2]);
      exit(1);
    }
    read_a = getc(file_a);
    read_b = getc(file_b);
    if (read_a != read_b) {
      printf(
          "Encountered difference after %lu characters (a: 0x%x, b: 0x%x)\n",
          num_chars + 1,
          read_a,
          read_b);
      exit(1);
    }
  }
  printf("Files are identical.\n");
  return 0;
}