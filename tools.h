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

#ifdef DEBUG
#define DEBUG_LOG(...)	fprintf(stderr, __VA_ARGS__);
#else
#define DEBUG_LOG(...)	do {} while (0)
#endif

void hash_file(char* path, off_t size, unsigned char* result);
uint16_t uint16_hash(unsigned char* buf, int len);
void print_hex(unsigned char* md, int len);

#endif	// _TOOLS_H
