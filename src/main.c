#include "main.h"

#include "config.h"
#include "split.h"

int main(const int argc, const char *argv[]) {
  struct Config cfg;
  initialise_config(&cfg);
  parse_config(&cfg, argc, argv);
  split_csv(&cfg);
}
