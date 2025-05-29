#include "rgba_io.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>

#define  STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

int read_rgba(const char *filename, uint32_t **buf, size_t *size)
{
	struct stat st;
	if (stat(filename, &st) == -1) {
		fprintf(stderr, "Error: Unable to collect %s stats.\n", filename);
		return -1;
	}
	*size = st.st_size;
	*buf = malloc(*size); //Allocate buffer to be large enough for size
	if (*buf == NULL) {
		fprintf(stderr, "Error: Unable to allocate enough space for the image buffer.\n");
		return -1;
	}
	int fd = open(filename, O_RDONLY);
	if (fd == -1) {	// If file descriptor error then free buffer
		free(*buf);
		*buf = NULL;
		fprintf(stderr, "Error: Unable to parse image file descriptor.\n");
		return -1;
	}
	ssize_t	bytes_read = read(fd, *buf, *size); // Assign bytes_read errchk
	close(fd);

	if (bytes_read < 0) {	// Error if bytes_read are negative. Will be unable to cast bytes_read to unsigned for comparison.
		free(*buf);
		*buf = NULL;
		fprintf(stderr, "Error: bytes_read should not be negative.\n");
		return -1;
	}

	if ((size_t)bytes_read != *size) { 
		free(*buf);
		*buf = NULL;
		fprintf(stderr, "Error: bytes_read does not match image size.");
		return -1;
	}

	if (*size % 4 != 0) {	// Check for RGBA format, sizes must be multiple of 4. 
		free(*buf);
		*buf = NULL;
		fprintf(stderr, "Error: '%s' has a size that is not a multiple of 4. Cannot be an RGBA.\n", filename);
		return -1;
	}

	return 0;
}

int read_image(const char *filename, uint32_t **buf, size_t *size)
{
	int width, height, channels;

	unsigned char *stb_data = stbi_load(filename, &width, &height, &channels, 4);

	if (stb_data != NULL) {		// Check for successful image data loading.
		*size = (size_t)width * (size_t)height * 4;
		*buf = malloc(*size);
		if (*buf == NULL) {	// Checks if malloc failed.
			stbi_image_free(stb_data);
			return -1;
		}
		memcpy(*buf, stb_data, *size);	// On success path, copy data to buffer.
		stbi_image_free(stb_data);
		return 0;
	} else {
		fprintf(stderr, "Warning: Could not load '%s' as PNG or JPG. Attempting RGBA read.\n", filename);
		return read_rgba(filename, buf, size);
	}
}

int write_rgba(const char *filename, uint32_t *buf, size_t size)
{
	int fd = open(filename, O_CREAT | O_WRONLY | O_TRUNC, 0644);
	if (fd == -1)
		return -1;
	ssize_t bytes_written = write(fd, buf, size);
	close(fd);

	if (bytes_written < 0) {
		return -1;		// Error if no bytes were written. 
	}

	if ((size_t)bytes_written != size)
		return -1;
	return 0;
}
