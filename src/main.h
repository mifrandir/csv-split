#ifndef MAIN_H
#define MAIN_H

#include "cli.h"
#include "log.h"

// Splits a CSV file based on the given config.
void split_csv(const struct Config *cfg);

#define FILE_NAME_SUFFIX_BUFFER 128

#endif