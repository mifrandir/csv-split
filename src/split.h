#ifndef SPLIT_H
#define SPLIT_H
#include <stdbool.h>
#define FILE_NAME_SUFFIX_BUFFER 128
#include "config.h"

// Splits a CSV file based on the given config.
void split_csv(const struct Config *cfg);

#endif