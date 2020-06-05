#define USAGE "test1 <BINARY> <DATA-DIR> <TEMP-DIR>"

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../log.h"
#include "compare_directories.h"

#define MAX_PATH_LEN 1024

struct Test {
  char *name;
  char *flags;
};

const char *binary_path, *data_dir_path, *temp_dir_path;

size_t extend_directory(const char *dir, const char *sub, char *buffer) {
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

static char *get_test_path(const char *dir, const char *test_name) {
  char *path = malloc((strlen(dir) + strlen(test_name) + 2) * sizeof(char));
  sprintf(path, "%s/%s", dir, test_name);
  return path;
}

// Tests that the get_out_path function works.
static int test_get_test_path() {
  printf("Running test \"%s\"...", "get_test_path");
  char *actual   = get_test_path("mydirectory", "mytest");
  char *expected = "mydirectory/mytest";
  int ret_val    = !!strcmp(actual, expected);
  if (ret_val) {
    printf("FAILED\n");
    ERR_LOG(
        "An error occurred while constructing the output path. (expected: %s, actual: "
        "%s)\n",
        expected,
        actual);
  }
  printf("PASSED\n");
  return ret_val;
}

static char *get_ref_path(const char *dir, const char *test_name) {
  char *path = malloc((strlen(dir) + strlen(test_name) + 6) * sizeof(char));
  sprintf(path, "%s/%s/out", dir, test_name);
  return path;
}

// Tests that the get_out_path function works.
static int test_get_ref_path() {
  printf("Running test \"%s\"...", "get_ref_path");
  char *actual   = get_ref_path("mydirectory", "mytest");
  char *expected = "mydirectory/mytest/out";
  int ret_val    = !!strcmp(actual, expected);
  if (ret_val) {
    printf("FAILED\n");
    ERR_LOG(
        "An error occurred while constructing the output path. (expected: %s, actual: "
        "%s)\n",
        expected,
        actual);
  }
  printf("PASSED\n");
  return ret_val;
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

// Tests that results using the default configuration are correct and consistent.
int test_default() {
  static struct Test test = {"default", ""};
  return run_test(&test);
}

int test_fewer_lines_than_split() {
  static struct Test test = {"fewer_lines_than_split", "-l 100"};
  return run_test(&test);
}

int test_all() {
  int err = 0;
  err |= test_get_ref_path();
  err |= test_get_test_path();
  err |= test_default();
  err |= test_fewer_lines_than_split();
  return err;
}

int main(const int argc, const char **argv) {
  if (argc != 4) {
    ERR_LOG("Expected 3 arguments, found %d.\nUsage: %s\n", argc - 1, USAGE);
    return 1;
  }
  binary_path   = argv[1];
  data_dir_path = argv[2];
  temp_dir_path = argv[3];
  if (access(binary_path, X_OK) == -1) {
    ERR_LOG("Missing permissions to execute the given binary. (%s)\n", binary_path);
    return 2;
  }
  if (test_all()) {
    printf("One or more tests failed.\n");
    return 1;
  }
  return 0;
}