#include "so_stdio.h"
#include "utils.h"
#include "my_defines.h"

/* creates the file data structure */
SO_FILE *so_fopen(const char *pathname, const char *mode)
{
	SO_FILE *so_file;
	HANDLE fd = get_file_descriptor_by_flag(pathname, mode);

	/* if there was an error while opening the file */
	if (fd == INVALID_HANDLE_VALUE) {
		return NULL;
	}

	so_file = (SO_FILE *)calloc(1, sizeof(SO_FILE));

	/* set data structure's variables */
	so_file->intern_offset = 0;
	so_file->fd = fd;
	so_file->len = 0;
	so_file->last_op = -1;
	so_file->read_size = 0;
	so_file->actual_read_size = 0;
	so_file->error = 0;
	so_file->eof = 0;
	return so_file;
}

/* closes the file data structure */
int so_fclose(SO_FILE *stream)
{
	int res;

	/* last operation was a WRITE so we need to write the data */
	if (stream->last_op == WRITE) {
		res = so_fflush(stream);
		if (res == SO_EOF) {
			free(stream);
			return SO_EOF;
		}
	}

	/* check for closing error */
	if (CloseHandle(stream->fd) == FALSE) {
		free(stream);
		return SO_EOF;
	}
	free(stream);
	return 0;
}

/* reads a byte from stream */
int so_fgetc(SO_FILE *stream)
{
	int ret;
	int character;
	int res;
	int bytes;

	/*if last op was a WRITE we need to flush the data */
	if (stream->last_op == WRITE) {
		res = so_fflush(stream);
		if (res == SO_EOF) {
			stream->error = 1;
			return SO_EOF;
		}

		/* reinitialize variables */
		stream->actual_read_size = 0;
		stream->intern_offset = 0;
		stream->len = 0;
		stream->read_size = 0;
	}

	/* we got to the end of stream buffer */
	if (stream->len == stream->intern_offset) {
		/* the buffer is full */
		if (stream->len == BUF_MAX_SIZE) {
			ret = ReadFile(stream->fd,
					stream->buf,
					BUF_MAX_SIZE,
					(LPDWORD)&bytes, NULL);
			if (ret == 0) {
				stream->error = 1;
				stream->eof = 1;
				return SO_EOF;
			}
			stream->read_size += bytes;
			stream->len = bytes;
			stream->intern_offset = 0;
		} else {
			/*try to read until the end of buffer */
			ret = ReadFile(stream->fd,
					stream->buf + stream->len,
					BUF_MAX_SIZE - stream->len,
					(LPDWORD)&bytes, NULL);
			if (ret == 0) {
				stream->eof = 1;
				stream->error = 1;
				return SO_EOF;
			}
			stream->read_size += bytes;
			stream->len += bytes;
		}
	}

	character = stream->buf[stream->intern_offset];
	stream->intern_offset += 1;
	stream->last_op = READ;
	stream->actual_read_size += 1;
	return (int)((unsigned char)character);
}

/* returns the file handle */
HANDLE so_fileno(SO_FILE *stream)
{
	return stream->fd;
}

/* reads multiple bytes */
size_t so_fread(void *ptr, size_t size, size_t nmemb, SO_FILE *stream)
{
	int i, j;
	unsigned char *buf =
		(unsigned char *) malloc(size * sizeof(unsigned char));
	int recv_char;
	unsigned char character;
	int number_of_elements = 0;
	int finish_loop = 0;
	int sum = 0;

	/*use so_fgetc in a loop */
	for (i = 0 ; i < nmemb ; i++) {
		memset(buf, 0, size);

		for (j = 0 ; j < size ; j++) {
			recv_char = so_fgetc(stream);
			if (recv_char == SO_EOF) {
				finish_loop = 1;
				stream->error = 1;
				stream->eof = 1;
				break;
			}
			character = (unsigned char)recv_char;

			buf[j] = (unsigned char)character;
			sum += 1;
		}
		if (finish_loop == 1)
			break;
		/* do the actual read to ptr at the desired position */
		memcpy((unsigned char *)ptr + (i * size), buf, size);
		number_of_elements += 1;
	}

	if (finish_loop == 1) {
		free(buf);
		return number_of_elements;
	}
	stream->last_op = READ;
	free(buf);
	return number_of_elements;
}

/* writes a byte to the file */
int so_fputc(int c, SO_FILE *stream)
{
	int sz, difference;
	int res;

	/* if the last op was a READ */
	if (stream->last_op == READ) {
		/* move the file offset */
		difference = stream->read_size - stream->actual_read_size;
		res = so_fseek(stream, -difference, SEEK_CUR);
		stream->actual_read_size = 0;
		stream->intern_offset = 0;
		stream->read_size = 0;
		stream->len = 0;
	}

	/* if the buffer is full, we need to write data to the actual file */
	if (stream->intern_offset == BUF_MAX_SIZE) {
		sz = so_fflush(stream);
		if (sz == -1) {
			stream->error = 1;
			return SO_EOF;
		}
		stream->intern_offset = 0;
		stream->read_size += BUF_MAX_SIZE;
	}

	stream->buf[stream->intern_offset] = c;
	stream->intern_offset += 1;
	stream->last_op = WRITE;
	stream->actual_read_size += 1;
	return c;
}

/* writes multiple bytes to the file */
size_t so_fwrite(const void *ptr, size_t size, size_t nmemb, SO_FILE *stream)
{
	int no_of_written_elements = 0;
	int i, j, putc_res;
	int error_break = 0;
	int idx;

	/* use so_fputc in a loop */
	for (i = 0 ; i < nmemb ; i++) {
		for (j = 0 ; j < size ; j++) {
			idx = j + i * size;
			putc_res =
				so_fputc(((unsigned char *) ptr)[idx], stream);
			if (putc_res == -1) {
				error_break = 1;
				stream->error = 1;
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

/* writes the buffer stream to the file */
int so_fflush(SO_FILE *stream)
{	int res;
	int bytes_to_write = stream->intern_offset;
	int written_bytes = 0;
	int aux;

	/* try to write until all of the data has been written */
	while (written_bytes != bytes_to_write) {
		res = WriteFile(stream->fd, stream->buf + written_bytes,
			bytes_to_write - written_bytes, (LPDWORD)&aux, NULL);
		if (res == FALSE) {
			stream->error = 1;
			return -1;
		}
		written_bytes += aux;
	}
	return 0;
}

/* returns the file offset */
long so_ftell(SO_FILE *stream)
{
	/* manage the difference between stream's offset and the file offset */
	int difference = stream->read_size - stream->actual_read_size;

	so_fseek(stream, -difference, SEEK_CUR);
	stream->read_size = 0;
	stream->actual_read_size = 0;
	/*use SetFilePointer to return the current offset */
	return SetFilePointer(stream->fd, 0, NULL, FILE_CURRENT);
}

/* changes the file offset */
int so_fseek(SO_FILE *stream, long offset, int whence)
{
	int res = SetFilePointer(stream->fd, offset, NULL, whence);

	/* error handler */
	if (res == INVALID_SET_FILE_POINTER) {
		stream->error = 1;
		return SO_EOF;
	}

	return 0;
}

/* tells if the file offset is at EOF */
int so_feof(SO_FILE *stream)
{
	return stream->eof;
}

/* tell if there was an error */
int so_ferror(SO_FILE *stream)
{
	return stream->error;
}

int so_pclose(SO_FILE *stream)
{
	return 0;
}

SO_FILE *so_popen(const char *command, const char *type)
{
	return NULL;
}
