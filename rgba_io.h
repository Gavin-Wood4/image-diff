#ifndef RGBA_IO_H
#define RGBA_IO_H

#include <stdint.h>
#include <stddef.h>

int read_rgba(const char *filename, uint32_t **buf, size_t *size);
int read_image(const char *filename, uint32_t **buf, size_t *size, int *width, int *height);
int write_image(const char *filename, uint32_t *buf, size_t size, int width, int height);
int write_rgba(const char *filename, uint32_t *buf, size_t size);
int write_png(const char *filename, uint32_t *buf, int width, int height);

#endif

