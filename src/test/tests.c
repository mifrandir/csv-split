#define USAGE "test1 <BINARY> <DATA-DIR> <TEMP-DIR>"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../log.h"
#include "run_test.h"

// Tests that the get_out_path function works.
int test_get_test_path() {
	printf("Running test \"%s\"...", "get_test_path");
	char *actual	 = get_test_path("mydirectory", "mytest");
	char *expected = "mydirectory/mytest";
	int ret_val		 = !!strcmp(actual, expected);
	if (ret_val) {
		printf("FAILED\n");
		ERR_LOG(
				"An error occurred while constructing the output path. (expected: %s, "
				"actual: "
				"%s)\n",
				expected,
				actual);
	}
	printf("PASSED\n");
	return ret_val;
}

// Tests that the get_out_path function works.
static int test_get_ref_path() {
	printf("Running test \"%s\"...", "get_ref_path");
	char *actual	 = get_ref_path("mydirectory", "mytest");
	char *expected = "mydirectory/mytest/out";
	int ret_val		 = !!strcmp(actual, expected);
	if (ret_val) {
		printf("FAILED\n");
		ERR_LOG(
				"An error occurred while constructing the output path. (expected: %s, "
				"actual: "
				"%s)\n",
				expected,
				actual);
	}
	printf("PASSED\n");
	return ret_val;
}

// Tests that results using the default configuration are correct and
// consistent.
static int test_default() {
	static struct Test test = {"default", ""};
	return run_test(&test);
}

// Tests that no files are generated if the split size is bigger than the number
// of lines and remainders are omitted.
static int test_fewer_lines_than_split() {
	static struct Test test = {"fewer_lines_than_split", "-l 100"};
	return run_test(&test);
}

// Tests that only one identical file is generated if the split size is bigger
// than the number of liens and remainders are included.
static int test_fewer_lines_than_split_with_remainders() {
	static struct Test test = {
			"fewer_lines_than_split_with_remainders",
			"-l 100 -i"};
	return run_test(&test);
}

// Tests that basic splitting with -l > 1 works.
static int test_basic_multiline_split() {
	static struct Test test = {"basic_multiline_split", "-l 2"};
	return run_test(&test);
}

// Tests that omitting the header lines works as intended.
static int test_basic_multiline_split_no_headers() {
	static struct Test test = {"basic_multiline_split_no_headers", "-l 2 -e"};
	return run_test(&test);
}

static int test_long_flags() {
	static struct Test test = {
			"long_flags",
			"--exclude-headers --line-count 2 --delimiter , --include-remainders"};
	return run_test(&test);
}

static int test_multiflags() {
	static struct Test test = {"multiflags", "-eil 2 -d ,"};
	return run_test(&test);
}

static int test_remove_single_column() {
	static struct Test test = {"remove_single_column", "-r BIN"};
	return run_test(&test);
}

int test_all() {
	int err = 0;
	err |= test_get_ref_path();
	err |= test_get_test_path();
	err |= test_default();
	err |= test_fewer_lines_than_split();
	err |= test_fewer_lines_than_split_with_remainders();
	err |= test_basic_multiline_split();
	err |= test_basic_multiline_split_no_headers();
	err |= test_long_flags();
	err |= test_multiflags();
	err |= test_remove_single_column();
	return err;
}

int main(int argc, char **argv) {
	if (argc != 4) {
		ERR_LOG("Expected 3 arguments, found %d.\nUsage: %s\n", argc - 1, USAGE);
		return 1;
	}
	if (access(argv[1], X_OK) == -1) {
		ERR_LOG("Missing permissions to execute the given binary. (%s)\n", argv[1]);
		return 2;
	}
	set_binary_path(argv[1]);
	set_data_dir_path(argv[2]);
	set_temp_dir_path(argv[3]);
	LOG("Running tests.\n");
	if (test_all()) {
		printf("One or more tests failed.\n");
		return 1;
	}
	return 0;
}