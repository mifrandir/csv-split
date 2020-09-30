#ifndef RUN_TEST_H
#define RUN_TEST_H

#define MAX_PATH_LEN 1024

struct Test {
  char *name;
  char *flags;
};

int run_test(struct Test *test);

int make_directory(const char *dir);

char *get_ref_path(const char *dir, const char *test_name);
char *get_test_path(const char *dir, const char *test_name);

void set_binary_path(char *p);
void set_data_dir_path(char *p);
void set_temp_dir_path(char *p);

#endif