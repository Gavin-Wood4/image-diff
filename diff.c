#include "rgba_io.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

/*
The purpose of this program is to subtract one image layer's RGB values from
another and output it's result in .rgba format. 
*/

typedef enum { ABS, SAT, MOD } diff_mode_t;

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
		fprintf(stderr, "Error: Could not read '%s'.\n", argv[1]);
		goto err;
	}
	if (read_rgba(argv[2], &img2, &size2) == -1) {
		fprintf(stderr, "Error: Could not read '%s'.\n", argv[2]);
		goto err;
	}

	if (size1 != size2) {
		fprintf(stderr, "Error: Images must be the same dimensions.\n");
		goto err;
	}
	diff_rgba(img1, img2, size1, mode);
	
	if (write_rgba(argv[3], img1, size1) == -1) {
		fprintf(stderr, "Error: Unable to write the new image.\n");
		goto err;	
	}

	free(img1);
	free(img2);

	return EXIT_SUCCESS;

err:	// Dumps memory if image reading or writing error. 
	if (img1) free(img1);
	if (img2) free(img2);
	fprintf(stderr, "There was an error with one of the image files.\n");
	return EXIT_FAILURE;
}

static void diff_rgba(uint32_t *img1, const uint32_t *img2, size_t size, diff_mode_t mode)
{
	size_t pxs = size / sizeof(uint32_t);	// 'pxs' is the number of pixels in the image.
	size_t i;
	uint32_t pix1, pix2, pixout;
	for (i = 0; i < pxs; ++i) {
		pix1 = img1[i];
		pix2 = img2[i];
		pixout = 0;
		int channel1, channel2, difference;
		uint8_t output_channel;
		
		int shift;
		for (shift = 0; shift <= 16; shift += 8) {
			channel1 = (pix1 >> shift) & 0xFF;	// This isolates one color channel from the desired pixel, R(shift = 0) G(shift = 8), and B (shift = 16).
			channel2 = (pix2 >> shift) & 0xFF;	// The alpha channel is skipped so that images are not transparent (unintended behavior). 
			difference = channel1 - channel2;

			switch (mode) {
				case SAT:
					output_channel = (difference < 0) ? 0 : (uint8_t)difference;	// If the computed difference is smaller than 0, clamp to 0.
					break;
				case MOD:
					output_channel = (uint8_t)difference;				// Implicit declaration to unsigned for modulo 256 subtraction.
					break;
				case ABS:
				default:
					output_channel = (difference < 0) ? (uint8_t)(-difference) : (uint8_t)difference;	// Change sign of difference to find absolute value if negative.
					break;
			}

			pixout |= (uint32_t)output_channel << shift;
		}

		img1[i] = pixout | 0xFF000000;	// Forces 100% opacity.
	}
}

