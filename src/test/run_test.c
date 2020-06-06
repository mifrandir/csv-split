#include "run_test.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../log.h"
#include "compare_directories.h"

char *binary_path, *data_dir_path, *temp_dir_path;

void set_binary_path(char *p) {
  binary_path = p;
}

void set_data_dir_path(char *p) {
  data_dir_path = p;
}

void set_temp_dir_path(char *p) {
  temp_dir_path = p;
}

static size_t extend_directory(const char *dir, const char *sub, char *buffer) {
  sprintf(buffer, "%s/%s", dir, sub);
  return strlen(dir) + strlen(sub) + 1;
}

int make_directory(const char *dir) {
  char *command = malloc((strlen(dir) + 10));
  sprintf(command, "mkdir -p %s", dir);
  int ret_val = system(command);
  free(command);
  return ret_val;
}

char *get_ref_path(const char *dir, const char *test_name) {
  char *path = malloc((strlen(dir) + strlen(test_name) + 6) * sizeof(char));
  sprintf(path, "%s/%s/out", dir, test_name);
  return path;
}

char *get_test_path(const char *dir, const char *test_name) {
  char *path = malloc((strlen(dir) + strlen(test_name) + 2) * sizeof(char));
  sprintf(path, "%s/%s", dir, test_name);
  return path;
}

int run_test(struct Test *test) {
  printf("Running test \"%s\"...", test->name);
  LOG("\n");   // Making formatting in debug mode a bit more pretty.
  char *ref_path  = get_ref_path(data_dir_path, test->name);
  char *test_path = get_test_path(temp_dir_path, test->name);
  int r           = make_directory(test_path);
  if (r == -1) {
    ERR_LOG("Creating temp directory at %s failed.\n", test_path);
    ERR_LOG("Error: %s\n", strerror(errno));
    printf("FAILED\n");
    return 1;
  }
  char *test_command = malloc(
      (128 + strlen(binary_path) + strlen(temp_dir_path) + strlen(data_dir_path) +
       strlen(test->flags)) *
      sizeof(char));
  sprintf(
      test_command,
      "%s -n %s/%s/split %s/%s/in.csv %s",
      binary_path,
      temp_dir_path,
      test->name,
      data_dir_path,
      test->name,
      test->flags);
  LOG("Running command: %s\n", test_command);
  if (system(test_command) == -1) {
    ERR_LOG(
        "Could not run command. Command: %s; Error: %s\n", test_command, strerror(errno));
    printf("FAILED\n");
    return 1;
  }
  free(test_command);
  LOG("Comparing results.\n");
  int ret_val = compare_directories(ref_path, test_path);
  free(ref_path);
  free(test_path);
  if (ret_val) {
    printf("FAILED\n");
  } else {
    printf("PASSED\n");
  }
  return ret_val;
}
