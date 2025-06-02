#include "image_io.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#define	 STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define	 STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

int read_rgba(const char *filename, uint32_t **buf, size_t *size)
{
	struct stat st;
	if (stat(filename, &st) == -1) {
		fprintf(stderr, "Error(%s): Unable to collect %s stats.\n", __func__, filename);
		return -1;
	}
	
	if (st.st_size < 0) {
		fprintf(stderr, "Error(%s): '%s' has an negative size.\n", __func__, filename);
		if (buf) *buf = NULL;
		return -1;
	}
	*size = (size_t)st.st_size;

	if (*size == 0) {
		fprintf(stderr, "Error(%s): Input file '%s' has a size of zero.\n", __func__, filename);
		if (buf) *buf = NULL;
		return -1;
	}
        if (*size % 4 != 0) {   // Check for RGBA format, sizes must be multiple of 4.
                if (buf) *buf = NULL;
                fprintf(stderr, "Error(%s): '%s' has a size that is not a multiple of 4. Cannot be an RGBA.\n", __func__, filename);
                return -1;
        }
	*buf = malloc(*size); //Allocate buffer to be large enough for size
	if (*buf == NULL) {
		fprintf(stderr, "Error(%s): Unable to allocate enough space for the image buffer.\n", __func__);
		return -1;
	}
	int fd = open(filename, O_RDONLY);
	if (fd == -1) {	// If file descriptor error then free buffer
		free(*buf);
		*buf = NULL;
		fprintf(stderr, "Error(%s): Unable to parse image file descriptor.\n", __func__);
		return -1;
	}
	ssize_t	bytes_read = read(fd, *buf, *size); // Assign bytes_read errchk
	if (close(fd) == -1) {
		fprintf(stderr, "Warning(%s): Error closing file '%s' after reading.\n", __func__, filename);
	}
	if (bytes_read < 0) {	// Error if bytes_read are negative. Will be unable to cast bytes_read to unsigned for comparison.
		free(*buf);
		*buf = NULL;
		fprintf(stderr, "Error(%s): bytes_read should not be negative.\n", __func__);
		return -1;
	}
	if ((size_t)bytes_read != *size) { 
		free(*buf);
		*buf = NULL;
		fprintf(stderr, "Error(%s): bytes_read does not match image size.\n", __func__);
		return -1;
	}
	return 0;
}

int read_image(const char *filename, uint32_t **buf, size_t *size, int *width, int *height)
{
	int lwidth, lheight, lchannels;		// Local variables

	if (width) *width = 0;
	if (height) *height = 0;

	unsigned char *stb_data = stbi_load(filename, &lwidth, &lheight, &lchannels, 4);

	if (stb_data != NULL) {		// Check for successful image data loading.
		*size = (size_t)lwidth * (size_t)lheight * 4;
		if (*size == 0) {
			fprintf(stderr, "Warning(%s): Image '%s' loaded with a size of zero.\n", __func__, filename);
			*buf = NULL;
			stbi_image_free(stb_data);
			if (width) {
				*width = lwidth;
			}
			if (height) {
				*height = lheight;
			}
			return 0;	// Successful image read but with no data buffer.
		}

		*buf = malloc(*size);
		if (*buf == NULL) {
			fprintf(stderr, "Error(%s): Failed to allocate image buffer while reading '%s'.\n", __func__, filename);
			stbi_image_free(stb_data);
			return -1;
		}

		memcpy(*buf, stb_data, *size);
		stbi_image_free(stb_data);

		if (width) {
			*width = lwidth;
		}
		if (height) {
			*height = lheight;
		}
		return 0;	// Successfull image read with data.
	} else {
		fprintf(stderr, "Warning(%s): Could not load '%s' as PNG or JPG with stb_image. Attempting RGBA read.\n", __func__, filename);
		return read_rgba(filename, buf, size);
	}
}

int write_rgba(const char *filename, uint32_t *buf, size_t size)
{
	if (size == 0) {
		fprintf(stderr, "Error(%s): Attempted to write 0 bytes to RGBA file '%s'.\n", __func__, filename);
		return -1;
	}
	int fd = open(filename, O_CREAT | O_WRONLY | O_TRUNC, 0644);
	if (fd == -1) {
		fprintf(stderr, "Error(%s): Unable to open or create '%s' for writing RGBA data.\n", __func__, filename);
		return -1;
	}
	ssize_t bytes_written = write(fd, buf, size);
	if (close(fd) == -1){
		fprintf(stderr, "Warning(%s): There was an error with closing '%s' after writing RGBA data.\n", __func__, filename);
	}
	if (bytes_written < 0) {
		fprintf(stderr, "Error(%s): Writing data to '%s' failed.\n", __func__, filename);
		return -1;		// Error if no bytes were written. 
	}
	if ((size_t)bytes_written != size) {
		fprintf(stderr, "Error(%s): Not all RGBA data was written to '%s'. Expected %zu, but wrote %zd.\n", __func__, filename, size, bytes_written);
		return -1;
	}
	return 0;
}

int write_png(const char *filename, uint32_t *buf, int width, int height)
{
	if ((width < 1) || (height < 1)) {
		fprintf(stderr, "Error(%s): Dimensions %dx%d for writing PNG '%s' are invalid.\n", __func__, width, height, filename);
		return -1;
	}

	int success = stbi_write_png(filename, width, height, 4, buf, width * 4);
	if (!success) {
		fprintf(stderr, "Error(%s), Failed to write PNG image to '%s'.\n", __func__, filename);
		return -1;
	}
	return 0;
}

int write_image(const char *filename, uint32_t *buf, size_t size, int width_output, int height_output) {
	if (!filename) {
		fprintf(stderr, "Error(%s): Image write called with NULL file name.\n", __func__);
		return -1;
	}
	if (!buf) {
		fprintf(stderr, "Error(%s): Image write called with NULL image buffer.\n", __func__);
		return -1;
	}
	if (size <= 0) {
		fprintf(stderr, "Error(%s): Image write called with an image size smaller than 1.\n", __func__);
	}

	size_t len = strlen(filename);

	if (len >= 4 && strcmp(filename + len - 4, ".png") == 0) {	// If the argument is greater than 4 characters, and the the four characters starting at address (filename + len - 4) are ".png"
		return write_png(filename, buf, width_output, height_output);
	} else if (len >= 4 && strcmp(filename + len - 4, "rgba") == 0) {
		return write_rgba(filename, buf, size);
	} else {
		fprintf(stderr, "Error: Unsupported output file type for '%s'.\n", filename);
		fprintf(stderr, "	Output filename must end with '.png' (with valid dimensions) or 'rgba'\n");
		return -1;
	}
}

