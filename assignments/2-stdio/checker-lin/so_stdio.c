#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include "so_stdio.h"
#include "utils.h"
#include "my_defines.h"

SO_FILE *so_fopen(const char *pathname, const char *mode)
{
	SO_FILE *so_file;
	int fd = get_file_descriptor_by_flag(pathname, mode);

	if (fd == -1)
		return NULL;
	so_file = calloc(1, sizeof(SO_FILE));

	if (strcmp(mode, "a+") == 0)
		so_file->read_and_append_mode = TRUE;
	else
		so_file->read_and_append_mode = FALSE;

	so_file->intern_offset = 0;
	so_file->fd = fd;
	so_file->len = 0;
	so_file->last_op = -1;
	so_file->read_size = 0;
	so_file->actual_read_size = 0;
	so_file->error = 0;

	return so_file;
}

int so_fclose(SO_FILE *stream)
{
	so_fflush(stream);
	if (close(stream->fd) < 0) {
		stream->error = 1;
		return SO_EOF;
	}
	free(stream);
	return 0;
}

int so_fgetc(SO_FILE *stream)
{
	int size;
	int character;
	int res;

	if (stream->last_op == WRITE) {
		res = so_fflush(stream);
		if (res == SO_EOF) {
			stream->error = 1;
			return SO_EOF;
		}
		stream->actual_read_size = 0;
		stream->intern_offset = 0;
		stream->len = 0;
		stream->read_size = 0;
		printf("\n\nread dupa write!!!!!!!!!!\n\n");
	}

	if (stream->len == stream->intern_offset) {
		if (stream->len == BUF_MAX_SIZE) {
			size = read(stream->fd, stream->buf, BUF_MAX_SIZE);
			if (size <= 0) {
				stream->error = 1;
				return SO_EOF;
			}
			stream->read_size += size;
			stream->len = size;
			stream->intern_offset = 0;
		} else {
			size = read(stream->fd, stream->buf + stream->len,
				BUF_MAX_SIZE - stream->len);
			if (size <= 0) {
				stream->error = 1;
				return SO_EOF;
			}
			stream->read_size += size;
			stream->len += size;
		}
	}

	character = stream->buf[stream->intern_offset];
	stream->intern_offset += 1;
	stream->last_op = READ;
	stream->actual_read_size += 1;
	return (int)((unsigned char)character);
}

int so_fileno(SO_FILE *stream)
{
	return stream->fd;
}

size_t so_fread(void *ptr, size_t size, size_t nmemb, SO_FILE *stream)
{
	int i, j;
	unsigned char buf[size];
	int recv_char;
	unsigned char character;
	int number_of_elements = 0;
	int finish_loop = 0;
	int sum = 0;

	for (i = 0 ; i < nmemb ; i++) {
		memset(buf, 0, size);

		for (j = 0 ; j < size ; j++) {
			recv_char = so_fgetc(stream);
			if (recv_char == SO_EOF) {
				finish_loop = 1;
				stream->error = 1;
				break;
			}
			character = (unsigned char)recv_char;

			buf[j] = (unsigned char)character;
			sum += 1;
		}
		if (finish_loop == 1)
			break;
		memcpy(ptr + (i * size), buf, size);
		number_of_elements += 1;
	}

	if (finish_loop == 1)
		return 0;
	stream->last_op = READ;
	return number_of_elements;
}

int so_fputc(int c, SO_FILE *stream)
{
	int sz, difference;
	int res;

	if (stream->last_op == READ) {
		difference = stream->read_size - stream->actual_read_size;
		res = so_fseek(stream, -difference, SEEK_CUR);
		stream->actual_read_size = 0;
		stream->intern_offset = 0;
		stream->read_size = 0;
		stream->len = 0;
	}

	if (stream->intern_offset == BUF_MAX_SIZE) {
		sz = so_fflush(stream);
		if (sz == -1)
			return SO_EOF;
		stream->intern_offset = 0;
	}

	stream->buf[stream->intern_offset] = c;
	stream->intern_offset += 1;
	stream->last_op = WRITE;
	stream->actual_read_size += 1;
	return c;
}

size_t so_fwrite(const void *ptr, size_t size, size_t nmemb, SO_FILE *stream)
{
	int no_of_written_elements = 0;
	int i, j, putc_res;
	int error_break = 0;
	int idx;

	for (i = 0 ; i < nmemb ; i++) {
		for (j = 0 ; j < size ; j++) {
			idx = j + i * size;
			putc_res = so_fputc(((unsigned char *) ptr)[idx], stream);
			if (putc_res == -1) {
				error_break = 1;
				break;
			}
		}

		if (error_break)
			break;
		no_of_written_elements += 1;
	}
	if (error_break == 1)
		return 0;
	return no_of_written_elements;
}

int so_fflush(SO_FILE *stream)
{	int sz;
	int bytes_to_write = stream->intern_offset;
	int bytes_wrote = 0;

	while (bytes_wrote != bytes_to_write) {
		sz = write(stream->fd, stream->buf, bytes_to_write - bytes_wrote);
		if (sz == -1)
			return -1;
		bytes_wrote += sz;
	}

	return 0;
}

long so_ftell(SO_FILE *stream)
{
	int difference = stream->read_size - stream->actual_read_size;

	so_fseek(stream, -difference, SEEK_CUR);
	stream->read_size = 0;
	stream->actual_read_size = 0;
	return lseek(stream->fd, 0, SEEK_CUR);
}

int so_fseek(SO_FILE *stream, long offset, int whence)
{
	int res = lseek(stream->fd, offset, whence);

	if (res == -1)
		return res;

	return 0;
}

int so_feof(SO_FILE *stream)
{

}

int so_ferror(SO_FILE *stream)
{

}

int so_pclose(SO_FILE *stream)
{

}

SO_FILE *so_popen(const char *command, const char *type)
{

}


int main(void)
{
	SO_FILE *file = so_fopen("fisier_test", "w");

	// so_fputc('a', file);
	// so_fputc('b', file);
	// so_fputc('c', file);
	// so_fputc('d', file);
	// so_fputc('e', file);
	// so_fputc('f', file);
	// so_fputc('g', file);
	// so_fputc('h', file);
	// so_fputc('i', file);
	// so_fputc('j', file);
	// so_fputc('k', file);
	// so_fputc('l', file);
	// so_fputc('m', file);
	// so_fputc('n', file);
	// so_fputc('o', file);
	// so_fputc('p', file);
	// so_fputc('q', file);
	// for (int i = 0 ; i < file->intern_offset ; i++)
	//	printf("%c", file->buf[i]);
	char *data = "12134tgrhbfvdc";

	/*so_fwrite(buf, 1, 200000, file);*/
	so_fclose(file);
}
