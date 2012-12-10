#ifndef _SNAPSHOT_H
#define _SNAPSHOT_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>

#include "tools.h"

typedef struct s_ssentry {
	char* path;
	int is_dir;
	int is_empty;
	unsigned char hash[MD5_DIGEST_LENGTH];
	off_t size;

	void* next_by_hash;	// link to the next SSENTRY in ht_by_hash
	void* next_by_path;	// link to the next SSENTRY in ht_by_path
} SSENTRY;

typedef struct s_sshashtable {
	uint16_t hash;
	SSENTRY* entries;	// link to the first SSENTRY for this hash

	void* next;	// link to the next element in this list of hashes
} SSHASHTABLE;

typedef struct s_snapshot {
	SSHASHTABLE* ht_by_hash;
	SSHASHTABLE* ht_by_path;
} SNAPSHOT;

SNAPSHOT* create_snapshot(char* path);
void destroy_snapshot(SNAPSHOT* ss);
void serialize_entry(SSENTRY* ssentry);
void serialize_snapshot(SNAPSHOT* ss);
SSENTRY* search_by_path(char* path, SNAPSHOT* ss);
SSENTRY* search_by_hash(unsigned char* longhash, SNAPSHOT* ss);

#endif	// _SNAPSHOT_H
