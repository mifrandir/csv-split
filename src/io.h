#ifndef IO_H
#define IO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "log.h"

#define DEFAULT_BUFFER_SIZE (1 << 20)

void io_read_open(const char *file_name);
ssize_t io_read_line(char **line, size_t *n);
void io_read_close(void);

int io_write_string(const char *s);
int io_write_char(const char c);
int io_write_open(const char *file_name);
int io_write_close(void);

#endif