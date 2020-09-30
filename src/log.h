#ifndef LOG_H
#define LOG_H
#include <stdio.h>

#include "debug.h"

#define ARG_ERR						-1
#define PARSE_ERR					-2
#define HELP_OUT					stderr
#define ERR_LOG(FMT, ...) fprintf(stderr, FMT, ##__VA_ARGS__)

#ifdef DEBUG
#define LOG(FMT, ...) fprintf(stderr, FMT, ##__VA_ARGS__)
#else
#define LOG(FMT, ...)
#endif

#endif