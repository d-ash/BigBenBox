#ifndef _TOOLS_H
#define _TOOLS_H

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/types.h>

#include <openssl/sha.h>

#ifdef PLATFORM_WINDOWS
#include "dirent.h"
#else
#include <dirent.h>
#endif // PLATFORM_WINDOWS

#ifdef PLATFORM_LINUX
#define PLATFORM_ID		0x01
#endif // PLATFORM_LINUX

#ifdef PLATFORM_WINDOWS
#define PLATFORM_ID		0x80
#endif // PLATFORM_WINDOWS

#define VERSION			0x0001

#define WORD_SIZE		sizeof(size_t)

#ifdef DEBUG
#define PLOG(...)	do { fprintf(stderr, __VA_ARGS__); } while (0);
#else
#define PLOG(...)	do {} while (0);
#endif

#define PERR(...)	do { fprintf(stderr, "ERROR: "); fprintf(stderr, __VA_ARGS__); } while (0);

typedef uint32_t checksum_t;

uint32_t uint32_hash(void* buf, size_t len);
uint16_t uint16_hash(void* buf, size_t len);
void update_checksum(void* buf, size_t len, checksum_t* checksum);
void print_hex(void* b, size_t len);
int sha256_file(char* path, unsigned char hash[SHA256_DIGEST_LENGTH]);

#endif	// _TOOLS_H
