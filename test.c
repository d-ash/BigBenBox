#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "hash.h"

int main(int argc, char* argv[]) {
	unsigned char s[3];
	char str[5] = "abcde";
	unsigned int h;

	s[0] = 0x01;
	s[1] = 0x02;
	s[2] = 0x03;
	h = uint16_hash(s, 3);
	printf("%0X\n", h);

	h = uint16_hash(str, strlen(str));
	printf("%0X\n", h);

	return 0;
}
