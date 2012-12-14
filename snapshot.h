#ifndef _SNAPSHOT_H
#define _SNAPSHOT_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>

#include "tools.h"

typedef uint16_t	hash_t;
#define HASH_MAX	UINT16_MAX

typedef struct s_ssentry_content {
	time_t mtime;
	off_t size;
} SSENTRY_CONTENT;

#define SSENTRY_STATUS_DIR	0x01

typedef struct s_ssentry_header {
	uint8_t status;
	SSENTRY_CONTENT content;
	size_t pathmem;		// strlen(path) + 1
} SSENTRY_HEADER;

typedef struct s_ssentry {
	void* next;			// link to the next SSENTRY

	SSENTRY_HEADER header;
	// WARNING: do not place anything after SSENTRY_HEADER
	// 'path' is stored here, just after header.
} SSENTRY;

#define SSENTRY_PATH(s)	((char*) s + sizeof(SSENTRY))

typedef struct s_snapshot {
	void* ht[HASH_MAX];		// hashtable of SSENTRY lists
} SNAPSHOT;

SNAPSHOT* create_snapshot(char* path);
void destroy_snapshot(SNAPSHOT* ss);

SSENTRY* search(char* path, SNAPSHOT* ss);

void process_dir(char* path, SNAPSHOT* ss);
void process_entry(char* path, char* name, SNAPSHOT* ss);

#endif	// _SNAPSHOT_H
