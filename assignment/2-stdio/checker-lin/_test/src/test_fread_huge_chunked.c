#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "so_stdio.h"
#include "test_util.h"

#include "hooks.h"

int num_sys_read;
int target_fd;

ssize_t hook_read(int fd, void *buf, size_t len);

struct func_hook hooks[] = {
	[0] = { .name = "read", .addr = (unsigned long)hook_read, .orig_addr = 0 },
};


//this will declare buf[] and buf_len
#include "huge_file.h"


ssize_t hook_read(int fd, void *buf, size_t len)
{
	ssize_t (*orig_read)(int, void *, size_t);

	orig_read = (ssize_t (*)(int, void *, size_t))hooks[0].orig_addr;

	if (fd == target_fd)
		num_sys_read++;

	return orig_read(fd, buf, len);
}


int main(int argc, char *argv[])
{
	SO_FILE *f;
	char *tmp;
	int ret;
	char *test_work_dir;
	char fpath[256];
	int total;
	int chunk_size = 2000;
	int to_read;
	int size_member;
	int total_members;

	tmp = malloc(buf_len);
	FAIL_IF(!tmp, "malloc failed\n");

	install_hooks("libso_stdio.so", hooks, 1);

	if (argc == 2)
		test_work_dir = argv[1];
	else
		test_work_dir = "_test";

	if (argc == 3)
		size_member = atoi(argv[2]);
	else
		size_member = 1;

	sprintf(fpath, "%s/huge_file", test_work_dir);

	ret = create_file_with_contents(fpath, buf, buf_len);
	FAIL_IF(ret != 0, "Couldn't create file: %s\n", fpath);


	/* --- BEGIN TEST --- */
	f = so_fopen(fpath, "r");
	FAIL_IF(!f, "Couldn't open file: %s\n", fpath);

	target_fd = so_fileno(f);

	num_sys_read = 0;	

	// read the rest of the file in chunks
	total = 0;
	while (total < buf_len) {
		if (total + chunk_size >= buf_len)
			to_read = buf_len - total;
		else
			to_read = chunk_size;

		total_members = to_read / size_member;
		ret = so_fread(&tmp[total], size_member, total_members, f);

		FAIL_IF(ret != total_members, "Incorrect return value for so_fread: got %d, expected %d\n", ret, total_members);

		total += total_members * size_member;
	}

	FAIL_IF(num_sys_read != 49, "Incorrect number of reads: got %d, expected %d\n", num_sys_read, 49);
	// unsigned char x;
	// for (int i = 0; i < buf_len; i++) {
	// 	x = tmp[i];
	// 	if (x != buf[i]) {
	// 		printf("i[%d] tmp[%d][%02x] <=> buf[%d][%02x]\n", i, x, x, buf[i], buf[i]);
	// 		break;
	// 	}
	// }


	FAIL_IF(memcmp(tmp, buf, buf_len), "Incorrect data\n");

	ret = so_fclose(f);
	FAIL_IF(ret != 0, "Incorrect return value for so_fclose: got %d, expected %d\n", ret, 0);

	free(tmp);

	return 0;
}
