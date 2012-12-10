#ifndef _TOOLS_H
#define _TOOLS_H

#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "md5.h"

#define MD5_DIGEST_LENGTH	16
#define HASH_SUM_LENGTH		16

// Wrapping all those MD5_Init, MD5_Update and MD5_Final
unsigned char* MD5(void* d, size_t n, unsigned char* md);

void hash_file(char* path, off_t size, unsigned char* result);

uint16_t uint16_hash(unsigned char* buf, int len);

// Print the hash sum as hex-digits.
void print_hash_sum(unsigned char* md);

#endif	// _TOOLS_H
