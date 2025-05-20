#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <arm_neon.h>

/*
The purpose of this program is to subtract one image layer's RGB values from
another and output it's result in .rgba format using NEON libraries
*/

int read_rgba(const char *filename, uint32_t **buf, size_t *size);
void diff_rgba(uint32_t *img1, const uint32_t *img2, size_t size);
int write_rgba(const char *filename, uint32_t *buf, size_t size);


int main(int argc, char *argv[])
{
	char arg_err_msg[] = "Incorrect number of arguments passed (must be 3.)";
	if (argc != 4)
		return printf(arg_err_msg);

	uint32_t *img1 = NULL;
	uint32_t *img2 = NULL;
	size_t size1, size2;

	if (read_rgba(argv[1], &img1, &size1) == -1) // next 6 lines err check
		goto err;
	if (read_rgba(argv[2], &img2, &size2) == -1)
		goto err;
	diff_rgba(img1, img2, size1);
	if (write_rgba(argv[3], img1, size1) == -1) {
		goto err;
	}
	return 0;	// For error checking compilation

err:	// Dump memory if image error
	free(img1);
	free(img2);
	return printf("There was an error with one of the image files.");
}

int read_rgba(const char *filename, uint32_t **buf, size_t *size)
{
	struct stat st;
	if (stat(filename, &st) == -1)
		return -1;
	*size = st.st_size;
	*buf = malloc(*size); //Allocate buffer to be large enough for size
	if (*buf == NULL) {
		return -1;
	}
	int fd = open(filename, O_RDONLY);
	if (fd == -1) {	// if file descriptor error then free buffer
		free(*buf);
		*buf = NULL;
		return -1;
	}
	ssize_t	bytes_read = read(fd, *buf, *size); // Assign bytes_read for err check
	close(fd);
	if (bytes_read != *size) { 
		free(*buf);
		*buf = NULL;
		return -1;
	}
	return 0;
}

void diff_rgba(uint32_t *img1, const uint32_t *img2, size_t size)
{
	size_t num_pixels = size / sizeof(uint32_t);
	size_t i = 0;	
	const uint32x4_t rgb_mask = vdupq_n_u32(0x00FFFFFF);
	const uint32x4_t a_mask = vdupq_n_u32(0xFF000000);

	for (; i + 3 < num_pixels; i+= 4) {
		uint32x4_t pix1 = vld1q_u32(img1 + i); // Holds 4 pixels
		uint32x4_t pix2 = vld1q_u32(img2 + i);
		uint32x4_t diff = vsubq_u32(pix1, pix2); // Find diff of 4 pixs	
		diff = vandq_u32(diff, rgb_mask);
		diff = vorrq_u32(diff, a_mask); // Set alpha bits full opacity
		vst1q_u32(img1 + i, diff); // Store modified pixels in memory
	}

	for (; i < num_pixels; i++) { // Process the last up to 3 pixels
		img1[i] = ((img1[i] - img2[i]) & 0x00FFFFFF) | 0xFF000000;
	}
}

int write_rgba(const char *filename, uint32_t *buf, size_t size) {
	int fd = open(filename, O_CREAT | O_WRONLY | O_TRUNC, 0644);
	if (fd == -1)
		return -1;
	ssize_t bytes_written = write(fd, buf, size);
	close(fd);
	if (bytes_written != size)
		return -1;
	free(buf);
	return 0;
}
