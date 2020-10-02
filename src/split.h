#ifndef SPLIT_H
#define SPLIT_H

#define LEN_FILE_NAME_SUFFIX_BUFFER 128
#define LEN_INITIAL_LINE_BUFFER 1024

#include "config.h"

// Splits a CSV file based on the given config.
void split_csv(const struct Config *cfg);

#endif