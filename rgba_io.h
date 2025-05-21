#ifndef RGBA_IO_H
#define RGBA_IO_H

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

int read_rgba(const char *filename, uint32_t **buf, size_t *size);
int write_rgba(const char *filename, uint32_t *buf, size_t size);

#endif
