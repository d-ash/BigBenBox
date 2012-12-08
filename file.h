#ifndef _FILE_H
#define _FILE_H

#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <openssl/md5.h>

void hash_file(char* path, off_t size, unsigned char* result);

#endif	// _FILE_H
