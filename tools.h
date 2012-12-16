#ifndef _TOOLS_H
#define _TOOLS_H

#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/types.h>

#define PLATFORM	0x0001
#define VERSION		0x0001

#ifdef DEBUG
#define PLOG(...)	do { fprintf(stderr, __VA_ARGS__); } while (0);
#else
#define PLOG(...)	do {} while (0);
#endif

#define PERR(...)	do { fprintf(stderr, "ERROR: "); fprintf(stderr, __VA_ARGS__); } while (0);

//void hash_file(char* path, off_t size, unsigned char* result);
uint16_t uint16_hash(void* buf, int len);
void print_hex(void* md, int len);

#endif	// _TOOLS_H
