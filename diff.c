#include "image_io.h"
#include "pix_diff.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

/*
The purpose of this program is to subtract one image layer's RGB values from
another and output it's result in .rgba format. 
*/

int main(int argc, char *argv[])
{
	if (argc < 4 || argc > 6) {
		fprintf(stderr, "Usage: %s <image1> <image2> <output.{png,rgba}> [absolute|abs|saturated|sat|modular|mod] [disable_neon]\n", argv[0]);
		return EXIT_FAILURE;
	}

	diff_mode_t mode = ABS;		// Set default mode to absolute.
	int disable_neon = 0;
	if (argc >= 5) {
		if ((strcmp(argv[4], "saturated") == 0)||(strcmp(argv[4], "sat") == 0)) {
			mode = SAT;
		} else if ((strcmp(argv[4], "modular") == 0)||(strcmp(argv[4], "mod") == 0)) {
			mode = MOD;
		} else if ((strcmp(argv[4], "absolute") == 0)||(strcmp(argv[4], "abs") == 0)) {
			mode = ABS;
		} else if (strcmp(argv[4], "disable_neon") == 0) {
			disable_neon = 1;
			if (argc == 6) {	// If argv[4] is 'disable_neon', nothing should be in argv[5].
				fprintf(stderr, "Error(%s): Invalid argument '%s' after '%s'.\n", __func__, argv[5], argv[4]);
				fprintf(stderr, "Usage: %s <image1> <image2> <output.{png,rgba}> [absolute|abs|saturated|sat|modular|mod] [disable_neon]\n", argv[0]);
				return EXIT_FAILURE;
			}
		}
		else {
			disable_neon = 1;
			fprintf(stderr, "Error(%s): Invalid fifth argument '%s'.\n", __func__, argv[4]);
			fprintf(stderr, "Usage: %s <image1> <image2> <output.{png,rgba}> [absolute|abs|saturated|sat|modular|mod] [disable_neon]\n", argv[0]);
			return EXIT_FAILURE;
		}
		if (argc == 6 && !disable_neon) {	// If argv[4] was disable_neon, argv[5] would not be checked for disable_neon here.
			if (strcmp(argv[5], "disable_neon") == 0) {
				disable_neon = 1;
			} else {
				fprintf(stderr, "Error(%s): Invalid sixth argument '%s'.\n)", __func__, argv[5]);
				fprintf(stderr, "Usage: %s <image1> <image2> <output.{png,rgba}> [absolute|abs|saturated|sat|modular|mod] [disable_neon]\n", argv[0]);
				return EXIT_FAILURE;
			}
		}
	}

	uint32_t *img1 = NULL;
	uint32_t *img2 = NULL;
	size_t size1, size2;
	int width1 = 0, height1 = 0;
	int width2 = 0, height2 = 0;

	if (read_image(argv[1], &img1, &size1, &width1, &height1) == -1) {
		fprintf(stderr, "Error(%s): Could not read '%s'.\n", __func__, argv[1]);
		goto err;
	}
	if (read_image(argv[2], &img2, &size2, &width2, &height2) == -1) {
		fprintf(stderr, "Error(%s): Could not read '%s'.\n", __func__, argv[2]);
		goto err;
	}

	if (size1 != size2) {
		fprintf(stderr, "Error(%s): Images must be the same dimensions.\n", __func__);
		goto err;
	}
	if (size1 == 0) {	// Sizes must be the same so only check size1.
		fprintf(stderr, "Error(%s): Input images have a size of 0, cannot subtract images.\n", __func__);
		goto err;
	}
	if ((img1 == NULL) || (img2 == NULL)) {
		fprintf(stderr, "Error(%s): Image buffer is NULL, despite a non-zero size after reading.\n", __func__);
		goto err;
	}

	if (((width1 != 0) && (height1 != 0) && (width2 != 0) && (height2 != 0)) &&	// Check for matching PNG input dimensions. This should only execute if two PNGs are provided. 
	     (width1 != width2 || height1 != height2)) {
		fprintf(stderr, "Error(%s): Image dimensions must be the same/non zero. '%s is %dx%d, and '%s' is %dx%d.\n",
			__func__, argv[1], width1, height1, argv[2], width2, height2);
		goto err;
	}

#ifdef __ARM_NEON
	if (disable_neon) {
		fprintf(stdout, "Info(%s): Using scalar differencing. NEON differencing disabled.\n", __func__);
		diff_scalar(img1, img2, size1, mode);
	} else {
		fprintf(stdout, "Info(%s): Using NEON differencing.\n", __func__);
		diff_neon(img1, img2, size1, mode);
	}
#else
	fprintf(stdout, "Info(%s): Using scalar differencing. (NEON differencing is not compiled.)\n", __func__);
#endif

	int width_for_png = 0, height_for_png = 0;

	if ((width1 != 0) && (height1 != 0)) {
		width_for_png = width1;
		height_for_png = height1;
	} else if ((width2 != 0) && (height2 != 0)) {
		width_for_png = width2;
		height_for_png = height2;
	}

	if (write_image(argv[3], img1, size1, width_for_png, height_for_png) == -1) {
		fprintf(stderr, "Error(%s): Failed to write to output image '%s'.", __func__, argv[3]);
		goto err;
	}

	free(img1);	// Dump image memory on success path.
	free(img2);

	return EXIT_SUCCESS;

err:	// Dumps memory if image reading or writing error. 
	if (img1) free(img1);
	if (img2) free(img2);
	fprintf(stderr, "Error(%s): Exiting due to failure.\n", __func__);	// There will be specific descriptive to the error messages above this from throughout the program.
	return EXIT_FAILURE;
}

