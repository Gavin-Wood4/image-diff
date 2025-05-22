#include "rgba_io.h"

/*
The purpose of this program is to subtract one image layer's RGB values from
another and output it's result in .rgba format. 
*/

void diff_rgba(uint32_t *img1, const uint32_t *img2, size_t size);

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

err:	// Dump memory if image error
	if (img1) free(img1);
	if (img2) free(img2);
	fprintf(stderr, "There was an error with one of the image files.\n");
	return EXIT_FAILURE;
}


void diff_rgba(uint32_t *img1, const uint32_t *img2, size_t size)
{
	size_t num_pixels = size/sizeof(uint32_t); // calculate number of pixels
	for (size_t i = 0; i < num_pixels; i++) {
		uint32_t pix1 = img1[i];
		uint32_t pix2 = img2[i];
		img1[i] = ((pix1 - pix2) & 0x00FFFFFF) | 0xFF000000; // Pix -
	}
}

