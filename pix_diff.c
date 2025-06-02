#include "pix_diff.h"
#include <stddef.h>
#include <stdint.h>

#ifdef __ARM_NEON
#include <arm_neon.h>
#endif

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

#ifdef __ARM_NEON
void diff_neon(uint32_t *img1, const uint32_t *img2, size_t size, diff_mode_t mode)
{
	size_t num_pixels = size / sizeof(uint32_t);			// The number of pixels in the image.
	const uint32x4_t alpha_only_mask = vdupq_n_u32(0xFF000000);	// Mask used to force output image opacity to 100%.
	
	uint8_t *img1_bytes = (uint8_t *)img1;				// Cast image pointers for byte iteration with NEON operations. 
	const uint8_t *img2_bytes = (const uint8_t *)img2;

	uint8x16_t neon_pxs1, neon_pxs2, neon_bytes_diff;		// Stores the data for 4 pixels each and the difference between them. 
	uint32x4_t neon_result;						// Stores the opacity corrected intermediate 4 pixels.
	
	size_t px_idx;
	for (px_idx = 0; px_idx + 3 < num_pixels; px_idx += 4) {
		neon_pxs1 = vld1q_u8(img1_bytes + px_idx * sizeof(uint32_t));	// Load 4 pixels * 32bits/pixel = 128 bits.
		neon_pxs2 = vld1q_u8(img2_bytes + px_idx * sizeof(uint32_t));

		switch (mode) {
			case ABS:
				neon_bytes_diff = vabdq_u8(neon_pxs1, neon_pxs2);	// |px1_channel - px2_channel|
				break;
			case SAT:
				neon_bytes_diff = vqsubq_u8(neon_pxs1, neon_pxs2);	// max(0, px1_channel - px2_channel)
				break;
			case MOD:
			default:
				neon_bytes_diff = vsubq_u8(neon_pxs1, neon_pxs2);	// (px1_channel - px2_channel) % 256
				break;
		}
		
		neon_result = vreinterpretq_u32_u8(neon_bytes_diff);
		neon_result = vorrq_u32(neon_result, alpha_only_mask);
		vst1q_u32(img1 + px_idx, neon_result);			// Store opacity corrected pixels.
	}

	for (; px_idx < num_pixels; px_idx++) { // Process the last up to 3 pixels
		img1[px_idx] = calculate_pixel_difference(img1[px_idx], img2[px_idx], mode);
	}
}
#endif

