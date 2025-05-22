#include "rgba_io.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>

/*
The purpose of this program is to subtract one image layer's RGB values from
another and output it's result in .rgba format. 
*/

static inline uint32_t abs_diff_pixel(uint32_t pix1, uint32_t pix2);
static void diff_rgba(uint32_t *img1, const uint32_t *img2, size_t size);

int main(int argc, char *argv[])
{
	if (argc != 4) {
		fprintf(stderr, "Usage: %s <image1> <image2> <output>\n", argv[0]);
		return EXIT_FAILURE;
	}

	uint32_t *img1 = NULL;
	uint32_t *img2 = NULL;
	size_t size1, size2;

	if (read_rgba(argv[1], &img1, &size1) == -1)
		goto err;
	if (read_rgba(argv[2], &img2, &size2) == -1)
		goto err;
	diff_rgba(img1, img2, size1);
	if (write_rgba(argv[3], img1, size1) == -1) {
		goto err;
	}
	return EXIT_SUCCESS;

err:	// Dumps memory if image reading or writing error.
	if (img1) free(img1);
	if (img2) free(img2);
	fprintf(stderr, "There was an error with one of the image files.\n");
	return EXIT_FAILURE;
}

static inline uint32_t abs_diff_pixel(uint32_t pix1, uint32_t pix2)
{
	uint32_t pixout = 0;
	int channel1, channel2, signed_difference;
	for (int shift = 0; shift < 32; shift += 8) {
		channel1 = (pix1 >> shift) & 0xFF;	// Extract the first color channel.
		channel2 = (pix2 >> shift) & 0xFF;
		signed_difference = channel1 - channel2;
		if (signed_difference < 0) signed_difference = -signed_difference;
		pixout |= (uint32_t)signed_difference << shift;		// Place pixel back in place.
	}
	return pixout;
}

static void diff_rgba(uint32_t *img1, const uint32_t *img2, size_t size)
{
	size_t i = 0;
	for (; i < size; i++)
		img1[i] = abs_diff_pixel(img1[i], img2[i]);
}

