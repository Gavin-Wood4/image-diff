#include "rgba_io.h"
#include "pix_diff.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <arm_neon.h>
#include <string.h>

/*
The purpose of this program is to subtract one image layer's RGB values from
another and output it's result in .rgba format using NEON libraries
*/


static void diff_rgba(uint32_t *img1, const uint32_t *img2, size_t size, diff_mode_t mode);

int main(int argc, char *argv[])
{
	if (argc != 4 && argc != 5) {
		fprintf(stderr, "Usage: %s <image1> <image2> <output> <absolute|abs|saturated|sat|modular|mod>\n", argv[0]);
		return EXIT_FAILURE;
	}

	diff_mode_t mode = ABS;
	if (argc == 5) {
		if ((strcmp(argv[4], "saturated") == 0)||(strcmp(argv[4], "sat") == 0)) {
			mode = SAT;
		} else if ((strcmp(argv[4], "modular") == 0)||(strcmp(argv[4], "mod") == 0)) {
			mode = MOD;
		} else if ((strcmp(argv[4], "absolute") == 0)||(strcmp(argv[4], "abs") == 0)) {
			mode = ABS;
		}
		else {
			fprintf(stderr, "Error: Invalid mode '%s'.\nUse: absolute (abs), saturated (sat), or modular (mod).\n", argv[4]);
			return EXIT_FAILURE;
		}
	}

	uint32_t *img1 = NULL;
	uint32_t *img2 = NULL;
	size_t size1, size2;

	if (read_rgba(argv[1], &img1, &size1) == -1) {
		fprintf(stderr, "Error: Could not read %s.\n", argv[1]);
		goto err;
	}
	if (read_rgba(argv[2], &img2, &size2) == -1) {
		fprintf(stderr, "Error: Could not read %s.\n", argv[2]);
		goto err;
	}
		
	if (size1 != size2) {
		fprintf(stderr, "Error: Images must be the same dimensions.\n");
		goto err;
	}
	diff_rgba(img1, img2, size1, mode);
	
	if (write_rgba(argv[3], img1, size1) == -1) {
		fprintf(stderr, "Error: Images must be the same dimensions.\n");
		goto err;
	}

	free(img1);
	free(img2);
	
	return EXIT_SUCCESS;

err:	// Dump memory if image error
	if (img1) free(img1);
	if (img2) free(img2);
	fprintf(stderr, "There was an error with one of the image files.\n");
	return EXIT_FAILURE;
}


static void diff_rgba(uint32_t *img1, const uint32_t *img2, size_t size, diff_mode_t mode)
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

