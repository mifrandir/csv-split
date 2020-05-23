#include "main.h"
int main(const int argc, const char *argv[]) {
  struct Config cfg;
  initialise_config(&cfg);
  parse_config(&cfg, argc, argv);
}