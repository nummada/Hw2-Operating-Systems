#include <stdio.h>
#include <string.h>

#include "utils.h"
#include "so_stdio.h"

/* returns a HANDLE on succes or INVALID_HANDLE_VALUE */
HANDLE get_file_descriptor_by_flag(const char *pathname, const char *mode)
{
	/* returns a HANDLE based on the file opening mode */

	if (strcmp(mode, "r") == 0)
		return CreateFile(pathname,
			GENERIC_READ,
			FILE_SHARE_READ,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL);

	if (strcmp(mode, "r+") == 0)
		return CreateFile(pathname,
			GENERIC_READ|GENERIC_WRITE,
			FILE_SHARE_READ|FILE_SHARE_WRITE,
			NULL,
			OPEN_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL);

	if (strcmp(mode, "w") == 0)
		return CreateFile(pathname,
			GENERIC_WRITE,
			FILE_SHARE_WRITE,
			NULL,
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL);

	if (strcmp(mode, "w+") == 0)
		return CreateFile(pathname,
			GENERIC_READ|GENERIC_WRITE,
			FILE_SHARE_READ|FILE_SHARE_WRITE,
			NULL,
			OPEN_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL);

	if (strcmp(mode, "a") == 0)
		return CreateFile(pathname,
			FILE_APPEND_DATA,
			FILE_SHARE_WRITE,
			NULL,
			OPEN_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL);

	if (strcmp(mode, "a+") == 0)
		return CreateFile(pathname,
			FILE_APPEND_DATA|GENERIC_READ,
			FILE_SHARE_READ|FILE_SHARE_WRITE,
			NULL,
			OPEN_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL);

	/* invalid mode */
	return INVALID_HANDLE_VALUE;
}
