#ifndef COMPARE_FILES_H
#define COMPARE_FILES_H

#include <stdio.h>
#include <stdlib.h>

#include "../log.h"

// Tries to open two files and compare them char by char. If reading fails or the contents
// diverge, a non-zero value is returned.
int compare_files(const char *path_a, const char *path_b) {
  LOG("Comparing files a: %s and b: %s\n", path_a, path_b);
  FILE *file_a = fopen(path_a, "r");
  if (file_a == NULL) {
    ERR_LOG("Failure opening %s: file does not exist.\n", path_a);
    return 1;
  }
  FILE *file_b = fopen(path_b, "r");
  if (file_b == NULL) {
    ERR_LOG("Failure opening %s: file does not exist.\n", path_b);
    return 1;
  }
  int read_a, read_b;
  for (size_t num_chars = 0; !feof(file_a) && !feof(file_b); num_chars++) {
    if (ferror(file_a)) {
      ERR_LOG("Failure reading file %s\n", path_a);
      return 1;
    }
    if (ferror(file_b)) {
      ERR_LOG("Failure reading file %s\n", path_b);
      return 1;
    }
    read_a = getc(file_a);
    read_b = getc(file_b);
    if (read_a != read_b) {
      printf(
          "Encountered difference after %lu characters (a: 0x%x, b: 0x%x)\n",
          num_chars + 1,
          read_a,
          read_b);
      fclose(file_a);
      fclose(file_b);
      return 1;
    }
  }
  LOG("Files are identical.\n");
  fclose(file_a);
  fclose(file_b);
  return 0;
}

#endif