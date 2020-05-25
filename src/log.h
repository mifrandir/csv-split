#ifndef LOG_H
#define LOG_H

#define DEBUG

#include <stdio.h>

#define ERR_LOG(FMT, ...) fprintf(stderr, FMT, ##__VA_ARGS__)

#ifdef DEBUG
#define LOG(FMT, ...) fprintf(stderr, FMT, ##__VA_ARGS__)
#else
#define LOG(FMT, ...)
#endif

#endif