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
	size_t	size;		// total memory used by all entries of this hash value (with paths)
} SSHASH_HEADER;

typedef struct s_snapshot {
	int restored;		// 0: dynamically generated, 1: restored from file
	SSHASH_HEADER* ht;	// hashtable
} SNAPSHOT;

int init_snapshot(SNAPSHOT* ss);
int destroy_snapshot(SNAPSHOT* ss);

int take_snapshot(char* path, SNAPSHOT* ss);
SSENTRY* search(char* path, SNAPSHOT* ss);

int process_dir(char* path, SNAPSHOT* ss);
int process_entry(char* path, char* name, SNAPSHOT* ss);
int add_to_snapshot(SSENTRY* ssentry, SNAPSHOT* ss);

#endif	// _SNAPSHOT_H
