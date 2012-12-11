#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "tools.h"

int main(int argc, char* argv[]) {
	unsigned char s[3];
	char str[6] = "abcde";
	unsigned int h;
	unsigned char md[HASH_SUM_LENGTH];

	s[0] = 0x01;
	s[1] = 0x02;
	s[2] = 0x03;
	h = uint16_hash(s, 3);
	printf("%0X\n", h);

	h = uint16_hash(str, strlen(str));
	printf("%0X\n", h);

	MD5(str, strlen(str), md);
	printf("MD5: ");
	print_hash_sum(md);
	printf("\nChk: ab56b4d92b40713acc5af89985d4b786");
	printf("\n");

	return 0;
}
