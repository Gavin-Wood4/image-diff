#include "pix_diff.h"

uint32_t calculate_pixel_difference(uint32_t pix1, uint32_t pix2, diff_mode_t mode)
{
	static uint32_t alpha_only_mask = 0xFF000000;
	uint32_t pixout = 0;
	int channel1, channel2, difference;
	uint8_t output_channel;

	int shift;
	for (shift = 0; shift <= 16; shift += 8) {		// Skips alpha channel
		channel1 = (pix1 >> shift) & 0xFF;		// This isolates one color channel from the desired pixel, R(shift = 0) G(shift = 8), and B (shift = 16).
		channel2 = (pix2 >> shift) & 0xFF;
		difference = channel1 - channel2;

		switch (mode) {
			case SAT:
				output_channel = (difference < 0) ? 0 : (uint8_t)difference;
				break;
			case MOD:
				output_channel = (uint8_t)difference;
				break;
			case ABS:
			default:
				output_channel = (difference < 0) ? (uint8_t)(-difference) : (uint8_t)difference;
				break;
		}

		pixout |= (uint32_t)output_channel << shift;
	}

	return pixout | alpha_only_mask;	// Forces 100% opacity. Otherwise, the result will assume img1's opacity levels which could be confusing.
}

void diff_scalar(uint32_t *img1, const uint32_t *img2, size_t size, diff_mode_t mode)
{
        size_t num_pixels = size / sizeof(uint32_t);

        size_t px_idx;
        for (px_idx = 0; px_idx < num_pixels; ++px_idx) {
                img1[px_idx] = calculate_pixel_difference(img1[px_idx], img2[px_idx], mode);
        }
}

