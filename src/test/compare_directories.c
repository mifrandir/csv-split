#include "compare_directories.h"

int main(const int argc, const char **argv) {
  if (argc != 3) {
    ERR_LOG("Expected two arguments to compare.\n");
  }
  return compare_directories(argv[1], argv[2]);
}
