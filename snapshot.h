#ifndef _SNAPSHOT_H
#define _SNAPSHOT_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>

#include "md5.h"
#include "tools.h"

#define FILEHASH_LENGTH		MD5_DIGEST_LENGTH

typedef struct s_ssentry_content {
	unsigned char filehash[FILEHASH_LENGTH];
	uint64_t filesize;
} SSENTRY_CONTENT;

#define SSENTRY_STATUS_DIR	0x01

typedef struct s_ssentry_header {
	uint32_t status;
	SSENTRY_CONTENT content;
	uint64_t pathmem;	// strlen(path) + 1
} SSENTRY_HEADER;

typedef struct s_ssentry {
	void* next_by_hash;	// link to the next SSENTRY in ht_by_hash
	void* next_by_path;	// link to the next SSENTRY in ht_by_path

	SSENTRY_HEADER header;

	// WARNING: do not place anything after SSENTRY_HEADER
	// 'path' is stored here, just after header.
} SSENTRY;

// to reference path use: SSENTRY_PATH(ssentry)
#define SSENTRY_PATH(s)	((char*) s + sizeof(SSENTRY))

typedef struct s_sshashtable {
	uint16_t hash;
	SSENTRY* entries;	// link to the first SSENTRY for this hash

	void* next;	// link to the next element in this list of hashes
} SSHASHTABLE;

typedef struct s_snapshot {
	SSHASHTABLE* ht_by_hash;
	SSHASHTABLE* ht_by_path;

	unsigned int entries_count;
	size_t pathmem_total;	// to be able to malloc all memory at once
} SNAPSHOT;

SNAPSHOT* create_snapshot(char* path);
void destroy_snapshot(SNAPSHOT* ss);

SSENTRY* search_by_path(char* path, SNAPSHOT* ss);
SSENTRY* search_by_filehash(unsigned char* longhash, SNAPSHOT* ss);

void process_dir(char* path, SNAPSHOT* ss);
void process_entry(char* path, char* name, SNAPSHOT* ss);

#endif	// _SNAPSHOT_H
