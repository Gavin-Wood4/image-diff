#ifndef PIX_DIFF_H
#define PIX_DIFF_H

#include <stdint.h>

typedef enum { ABS, SAT, MOD } diff_mode_t;

uint32_t calculate_pixel_difference(uint32_t pix1, uint32_t pix2, diff_mode_t mode);

#endif
