#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

int main() {

	FILE *file = fopen("myinput", "r+");
	char buf[10];
	int sz = fread(buf, sizeof(char), 50, file);
	printf("cat a citit de fapt: [%d]\n", sz);
	// fwrite("BANANA", sizeof(char), 6, file);
	// fwrite("X", sizeof(char), 1, file);
	fclose(file);

	// int fd = open("myinput", O_RDWR, 0644); 
	// char buf[10];
	// memset(buf, 0, 10);
	
	// int sz = read(fd, buf, 50);
	// printf("cat a citit de fapt: [%d]\n", sz);
	// int sz2 = write(fd, "BANANA", 6);
	// printf("[%s]\n", buf);
	// sz = read(fd, buf, 1);
	// sz2 = write(fd, "X", 1);
	return 0;
}