#ifndef SPLIT_H
#define SPLIT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "config.h"
#include "io.h"
#include "log.h"

#define FILE_NAME_SUFFIX_BUFFER 128

// Splits a CSV file based on the given config.
void split_csv(const struct Config *cfg);

#endif