#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

#include "utils.h"
#include "so_stdio.h"

int get_file_descriptor_by_flag(const char *pathname, const char *mode)
{
	if (strcmp(mode, "r") == 0)
		return open(pathname, O_RDONLY, 0644);

	if (strcmp(mode, "r+") == 0)
		return open(pathname, O_RDWR, 0644);

	if (strcmp(mode, "w") == 0)
		return open(pathname, O_WRONLY | O_CREAT | O_TRUNC, 0644);

	if (strcmp(mode, "w+") == 0)
		return open(pathname, O_RDWR | O_CREAT | O_TRUNC, 0644);

	if (strcmp(mode, "a") == 0)
		return open(pathname, O_WRONLY | O_CREAT | O_APPEND, 0644);

	if (strcmp(mode, "a+") == 0)
		return open(pathname, O_RDWR | O_CREAT | O_APPEND, 0644);

	return -1;
}
