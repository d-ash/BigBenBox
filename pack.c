#include <sys/stat.h>
#include "pack.h"

void construct_pfh(unsigned char* pfh /* PACKFILE_HEADER_SIZE */) {
	pfh[0] = PACKFILE_MAGIC;
	pfh[1] = (is_little_endian() ? 1 : 0) | WORD_SIZE;
	pfh[2] = PLATFORM_ID;
	pfh[3] = PACKFILE_STRUCT_VER;
}

int save_snapshot(char* path, SNAPSHOT* ss) {
	FILE* f;
	unsigned char pfh[PACKFILE_HEADER_SIZE];
	int res = 0;

	f = fopen(path, "wb");
	if (f == NULL) {
		PERR("Cannot write a snapshot to %s: %s\n", path, strerror(errno));
		return 0;
	}

	construct_pfh(pfh);

	if (fwrite(pfh, sizeof(pfh), 1, f) == 1) {
		res = pack_snapshot(f, ss);
	} else {
		PERR("Cannot write a header to the snapshot file %s: %s\n", path, strerror(errno));
	}

	if (fclose(f) != 0) {
		PERR("Cannot save a snapshot to %s: %s\n", path, strerror(errno));
		return 0;
	}

	if (!res) {
		unlink(path);
	}

	return res;
}

int load_snapshot(char* path, SNAPSHOT* ss) {
	FILE* f = NULL;
	unsigned char pfh[PACKFILE_HEADER_SIZE];
	unsigned char pfh_control[PACKFILE_HEADER_SIZE];
	int res = 0;

	if (ss == NULL) {
		PERR("NULL value in %s()\n", __FUNCTION__);
		return 0;
	}

	init_snapshot(ss);
	ss->restored = 1;

	f = fopen(path, "rb");
	if (f == NULL) {
		PERR("Cannot open %s: %s\n", path, strerror(errno));
		destroy_snapshot(ss);
		return 0;
	}

	if (fread(pfh, sizeof(pfh), 1, f) < 1) {
		PERR("Cannot read from %s: %s\n", path, strerror(errno));
		destroy_snapshot(ss);
		return 0;
	}

	construct_pfh(pfh_control);

	// We can correctly read files only created with this same program on this machine.
	if (memcmp(pfh, pfh_control, sizeof(pfh)) == 0) {
		res = unpack_snapshot(f, ss);
	} else {
		PERR("Header of the snapshot file %s is incorrect.\n", path);
	}

	if (fclose(f) != 0) {
		PERR("Cannot close %s: %s\n", path, strerror(errno));
		destroy_snapshot(ss);
		return 0;
	}

	if (!res) {
		destroy_snapshot(ss);
		return 0;
	}

	return 1;
}

int pack_snapshot(FILE* f, SNAPSHOT* ss) {
	hash_t i;
	SSENTRY* ssentry = NULL;
	SSHASH_HEADER* cur_hh = NULL;
	PACK_HASH_HEADER h;

	for (i = 0; i < HASH_MAX; i++) {
		cur_hh = & ss->ht[i];

		if (cur_hh->first == NULL) {
			// Do not store hashes with no entries, it's packing anyway.
			continue;
		}

		h.hash = i;
		h.size = cur_hh->size;

		if (fwrite(&h, sizeof(h), 1, f) < 1) {
			return 0;
		}

		ssentry = cur_hh->first;
		do {
			if (fwrite(ssentry, sizeof(SSENTRY) + ssentry->pathmem, 1, f) < 1) {
				return 0;
			}
			ssentry = ssentry->next;
		} while (ssentry != NULL);
	}

	return 1;
}

int unpack_snapshot(FILE* f, SNAPSHOT* ss) {
	PACK_HASH_HEADER h;
	SSENTRY* ssentry = NULL;
	SSHASH_HEADER* cur_hh = NULL;
	unsigned char* max_next = NULL;

	// iterating over the hash list
	while (fread(&h, sizeof(h), 1, f) == 1) {

		// allocating memory for all entries with this hash
		cur_hh = & ss->ht[h.hash];
		cur_hh->size = h.size;
		cur_hh->first = malloc(h.size);

		if (cur_hh->first == NULL) {
			PERR("Cannot allocate memory for a entries list: %s\n", strerror(errno));
			return 0;
		}

		// The highest possible pointer value (counting not empty string).
		max_next = (unsigned char*) cur_hh->first + h.size - sizeof(SSENTRY) - 2;

		if (fread(cur_hh->first, h.size, 1, f) < 1) {
			PERR("Cannot read from a snapshot file: %s\n", strerror(errno));
			return 0;
		}

		// Set correct values for SSENTRY.next, all restored pointers are incorrect,
		// but we are searching for NULL value!
		ssentry = cur_hh->first;
		while (ssentry->next != NULL) {
			ssentry->next = (unsigned char*) ssentry + sizeof(SSENTRY) + ssentry->pathmem;

			if ((unsigned char*) ssentry->next > max_next) {
				PERR("Snapshot file is corrupted!\n");
				return 0;
			}

			ssentry = ssentry->next;
		}
	}

	if (!feof(f)) {
		PERR("Snapshot file is inconsistent (size of the file)\n");
		return 0;
	}

	return 1;
}
