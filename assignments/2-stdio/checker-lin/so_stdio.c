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

	return so_file;
}

int so_fclose(SO_FILE *stream)
{
	if (close(stream->fd) < 0)
		return SO_EOF;
	free(stream);
	return 0;
}

int so_fgetc(SO_FILE *stream)
{	
	int size;
	int character;

	if(stream->last_op == WRITE) {
		printf("\n\n read dupa write!!!!!!!!!! \n\n");
	}

	if (stream->len == stream->intern_offset) {
		if(stream->len == BUF_MAX_SIZE) {
			size = read(stream->fd, stream->buf, BUF_MAX_SIZE);
			if (size <= 0){
				// printf("eroareeeeeeeeeeeee");
				return SO_EOF;
			}
			stream->len = size;
			stream->intern_offset = 0;
		} else {
			size = read(stream->fd, stream->buf + stream->len, BUF_MAX_SIZE - stream->len);
			// printf("size: [%d]\n", size);
			if (size <= 0){
				// printf("eroareeeeeeeeeeeee");
				return SO_EOF;
			}
			stream->len += size;
			// printf("[%d][%d]\n", stream->len, stream->intern_offset);
		}
	}

	character = stream->buf[stream->intern_offset];
	stream->intern_offset += 1;
	stream->last_op = READ;
	return (int)((unsigned char)character);
}

int so_fileno(SO_FILE *stream)
{
	return stream->fd;
}

size_t so_fread(void *ptr, size_t size, size_t nmemb, SO_FILE *stream)
{
	int i, j;
	char buf[size];
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
				printf("ok");
				finish_loop = 1;
				break;
			} else {
				character = (unsigned char)recv_char;
			}
			// printf("%x ", character);
			buf[j] = (unsigned char)character;
			sum += 1;
		}
		if (finish_loop == 1)
			break;
		strncpy(ptr + (i * size), buf, size);
		number_of_elements += 1;
	}

	if(finish_loop == 1) return 0;
	// printf("[%d]\n", sum);
	return number_of_elements;

}

int so_fputc(int c, SO_FILE *stream)
{

}
size_t so_fwrite(const void *ptr, size_t size, size_t nmemb, SO_FILE *stream)
{

}

int so_fflush(SO_FILE *stream)
{

}

long so_ftell(SO_FILE *stream)
{

}

int so_fseek(SO_FILE *stream, long offset, int whence)
{

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
	SO_FILE *file = so_fopen("myinput", "r+");
	char* ptr;
	int elemente_size = 4;
	int no_of_elements = 500;
	ptr = calloc(no_of_elements, elemente_size);
	int no = so_fread(ptr, elemente_size, no_of_elements, file);
	// printf("[%s]\n", ptr);
	printf("[%d]\n", no);
	// printf("numarul de elemente:[%d], buffer: [%s]", no, ptr);
	
	
	// int characater = so_fgetc(file);
	// printf("[%c]\n", characater);
	// characater = so_fgetc(file);
	// printf("[%c]\n", characater);
	// characater = so_fgetc(file);
	// printf("[%c]\n", characater);
	// characater = so_fgetc(file);
	// printf("[%c]\n", characater);
	// characater = so_fgetc(file);
	// printf("[%c]\n", characater);
	// characater = so_fgetc(file);
	// printf("[%c]\n", characater);
	// characater = so_fgetc(file);
	// printf("[%c]\n", characater);
	// characater = so_fgetc(file);
	// printf("[%c]\n", characater);
	// characater = so_fgetc(file);
	// printf("[%c]\n", characater);
	// characater = so_fgetc(file);
	// printf("[%c]\n", characater);
	// characater = so_fgetc(file);
	// printf("[%c]\n", characater);
	
	so_fclose(file);

}
