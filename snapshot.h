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

typedef struct s_ssentry {
	uint8_t status;
	SSENTRY_CONTENT content;
	size_t pathmem;		// strlen(path) + 1

	void* next;			// link to the next SSENTRY
} SSENTRY;
// WARNING: 'path' is stored here, just after SSENTRY.

#define SSENTRY_PATH(s)	((char*) s + sizeof(SSENTRY))

typedef struct s_sshash_header {
	SSENTRY* first;
	size_t	size;	// total memory used by all entries of this hash value (with paths)
} SSHASH_HEADER;

typedef SSHASH_HEADER* snapshot_t;	// hashtable

snapshot_t create_snapshot();
void destroy_snapshot(snapshot_t ss);

snapshot_t generate_snapshot(char* path);
SSENTRY* search(char* path, snapshot_t ss);

void process_dir(char* path, snapshot_t ss);
void process_entry(char* path, char* name, snapshot_t ss);

#endif	// _SNAPSHOT_H
