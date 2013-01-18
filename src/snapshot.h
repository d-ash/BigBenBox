/**
 * Snapshot can be restored or generated manually.
 * Restored snapshot are a special case when memory for entries
 * is allocated in one huge chunk (at SNAPSHOT.ht->first).
 * So destroying must be kept according to the flag SNAPSHOT.restored.
 * And adding to this kind of snapshots is prohibited.
 *
 * In the future this can be reimplemented, so that every entry would have a separate flag.
 * This will allow to manipulate with restored snapshots as with normal.
 * And it will be possible to compact a memory usage by sequential saving & loading of snapshot.
 */

#ifndef _BBB_SNAPSHOT_H
#define _BBB_SNAPSHOT_H

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
	uint8_t custom;				// can be used by used code, at the end it has to be reset to 0
	SSENTRY_CONTENT content;
	size_t pathmem;				// strlen(path) + 1 + additional bytes for memory alignment
	void* next;					// link to the next SSENTRY
} SSENTRY;
// WARNING: 'path' is stored here, just after SSENTRY.
// sizeof(SSENTRY) must be multiple of WORD_SIZE

#define SSENTRY_PATH(s)	((char*) s + sizeof(SSENTRY))

typedef struct s_sshash_header {
	SSENTRY* first;
	size_t	size;				// total memory used by all entries of this hash value (with paths)
} SSHASH_HEADER;

typedef struct s_snapshot {
	int restored;				// 0: dynamically generated, 1: restored from file
	char* tf_path;				// path where this snapshot was taken from
	SSHASH_HEADER* ht;			// hashtable
} SNAPSHOT;

int init_snapshot(SNAPSHOT* ss);
int destroy_snapshot(SNAPSHOT* ss);

int take_snapshot(char* path, SNAPSHOT* ss);
SSENTRY* search(char* path, SNAPSHOT* ss);

int process_dir(char* path, size_t skip, SNAPSHOT* ss);
int process_entry(char* path, size_t skip, char* name, SNAPSHOT* ss);
int add_to_snapshot(SSENTRY* ssentry, SNAPSHOT* ss);

int find_changes(SNAPSHOT* ss0, SNAPSHOT* ss1);

#endif
