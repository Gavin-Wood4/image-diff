#ifndef PIX_DIFF_H
#define PIX_DIFF_H

#include <stdint.h>
#include <stddef.h>

typedef enum { ABS, SAT, MOD } diff_mode_t;

uint32_t calculate_pixel_difference(uint32_t pix1, uint32_t pix2, diff_mode_t mode);
void diff_scalar(uint32_t *img1, const uint32_t *img2, size_t size, diff_mode_t mode);

#endif

