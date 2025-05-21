#include "rgba_io.h"

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
	if (fd == -1) {	// If file descriptor error then free buffer
		free(*buf);
		*buf = NULL;
		return -1;
	}
	ssize_t	bytes_read = read(fd, *buf, *size); // Assign bytes_read errchk
	close(fd);
	if (bytes_read != *size) { 
		free(*buf);
		*buf = NULL;
		return -1;
	}
	return 0;
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
