#ifndef COMPARE_DIRECTORIES_H
#define COMPARE_DIRECTORIES_H

#include <dirent.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "../log.h"
#include "compare_files.h"

#define MAX_FILE_COUNT 1024
#define DT_FILE        8

ssize_t read_file_names(const char *path, char **buffer) {
  DIR *dir = opendir(path);
  if (dir == NULL) {
    ERR_LOG("Failure opening directory. (%s)\n", path);
    return -1;
  }
  struct dirent *ent;
  char **p = buffer;
  while ((ent = readdir(dir)) != NULL) {
    if (ent->d_type == DT_FILE) {
      *(p++) = ent->d_name;
    }
  }
  closedir(dir);
  return p - buffer;
}

static int compare_dir_by_list(
    const char *dir_a,
    const char *dir_b,
    size_t num_files,
    char **file_names) {
  size_t len_a = strlen(dir_a);
  size_t len_b = strlen(dir_b);
  // We need to allocate this from heap memory to avoid a stack overflow.
  char *path_a = malloc((FILENAME_MAX + len_a + 2) * sizeof(char));
  char *path_b = malloc((FILENAME_MAX + len_b + 2) * sizeof(char));
  strcpy(path_a, dir_a);
  strcpy(path_b, dir_b);
  path_a[len_a++]         = '/';
  path_b[len_b++]         = '/';
  char *file_name_begin_a = path_a + len_a;
  char *file_name_begin_b = path_b + len_b;
  for (size_t i = 0; i < num_files; i++) {
    strcpy(file_name_begin_a, file_names[i]);
    strcpy(file_name_begin_b, file_names[i]);
    if (compare_files(path_a, path_b)) {
      printf(
          "Equality of directories could not be confirmed. Diverging "
          "files:\n%s\n%s\n",
          path_a,
          path_b);
      free(path_a);
      free(path_b);
      return 1;
    }
  }
  free(path_a);
  free(path_b);
  return 0;
}

static int cmp(const void *s1, const void *s2) {
  return strcmp((char *)s1, (char *)s2);
}

int compare_directories(const char *dir_a, const char *dir_b) {
  char *file_names_a[MAX_FILE_COUNT];
  ssize_t num_files_a = read_file_names(dir_a, file_names_a);
  if (num_files_a < 0) {
    return 1;
  }
  LOG("Read %lu files from %s\n", num_files_a, dir_a);
  char *file_names_b[MAX_FILE_COUNT];
  ssize_t num_files_b = read_file_names(dir_b, file_names_b);
  if (num_files_b < 0) {
    return 1;
  }
  LOG("Read %lu files from %s\n", num_files_b, dir_b);
  if (num_files_a != num_files_b) {
    ERR_LOG(
        "Directories have different number of files. (a: %lu, b: %lu)\n",
        num_files_a,
        num_files_b);
    return 1;
  }
  qsort(file_names_a, num_files_a, sizeof(char *), cmp);
  qsort(file_names_b, num_files_b, sizeof(char *), cmp);
  for (size_t i = 0; i < num_files_a; i++) {
    if (strcmp(file_names_a[i], file_names_b[i])) {
      ERR_LOG(
          "Diverging file names for file #%lu in directories. (a: %s, b: %s)\n",
          i + 1,
          file_names_a[i],
          file_names_b[i]);
      return 1;
    }
  }
  return compare_dir_by_list(dir_a, dir_b, num_files_a, file_names_a);
}

#endif