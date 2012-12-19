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
	PACKFILE_HEADER_EXT pfh_ext;
	int res = 0;
	checksum_t checksum = 0;

	f = fopen(path, "wb");
	if (f == NULL) {
		PERR("Cannot write a snapshot to %s: %s\n", path, strerror(errno));
		return 0;
	}

	construct_pfh(pfh);
	if (fwrite(pfh, sizeof(pfh), 1, f) < 1) {
		PERR("Cannot write a header to the snapshot file %s: %s\n", path, strerror(errno));
		return 0;
	}
	update_checksum(pfh, sizeof(pfh), &checksum);

	pfh_ext.tf_pathmem = strlen(ss->tf_path) + 1;
	if (fwrite(&pfh_ext, sizeof(pfh_ext), 1, f) < 1) {
		PERR("Cannot write an extended header to the snapshot file %s: %s\n", path, strerror(errno));
		return 0;
	}
	update_checksum(&pfh_ext, sizeof(pfh_ext), &checksum);

	if (fwrite(ss->tf_path, pfh_ext.tf_pathmem, 1, f) < 1) {
		PERR("Cannot write tf_path to the snapshot file %s: %s\n", path, strerror(errno));
		return 0;
	}
	update_checksum(ss->tf_path, pfh_ext.tf_pathmem, &checksum);

	res = pack_snapshot(f, ss, &checksum);

	// checksum is saved in a network order
	checksum = htonl(checksum);
	if (fwrite(&checksum, sizeof(checksum), 1, f) < 1) {
		PERR("Cannot write a checksum to the snapshot file %s: %s\n", path, strerror(errno));
		return 0;
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
	PACKFILE_HEADER_EXT pfh_ext;
	int res = 0;
	checksum_t checksum = 0;
	checksum_t checksum_read = 0;

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
	update_checksum(pfh, sizeof(pfh), &checksum);

	construct_pfh(pfh_control);
	if (memcmp(pfh, pfh_control, sizeof(pfh)) != 0) {
		PERR("Header of the snapshot file %s is incorrect.\n", path);
		destroy_snapshot(ss);
		return 0;
	}

	// Reading extended header. Platform dependent types are already in use!
	// We can correctly read files only created with this same program on this machine.
	if (fread(&pfh_ext, sizeof(pfh_ext), 1, f) < 1) {
		PERR("Cannot read an extended header from a snapshot file: %s\n", strerror(errno));
		destroy_snapshot(ss);
		return 0;
	}
	update_checksum(&pfh_ext, sizeof(pfh_ext), &checksum);

	ss->tf_path = malloc(pfh_ext.tf_pathmem);
	if (ss->tf_path == NULL) {
		PERR("Cannot allocate memory: %s\n", strerror(errno));
		destroy_snapshot(ss);
		return 0;
	}

	if (fread(ss->tf_path, pfh_ext.tf_pathmem, 1, f) < 1) {
		PERR("Cannot read tf_path from a snapshot file: %s\n", strerror(errno));
		destroy_snapshot(ss);
		return 0;
	}
	update_checksum(ss->tf_path, pfh_ext.tf_pathmem, &checksum);

	res = unpack_snapshot(f, ss, &checksum);

	// checksum will be overread by the previous fread()
	if (fseek(f, 0 - sizeof(checksum_read), SEEK_END) != 0) {
		PERR("Cannot fseek() to a checksum of the file %s: %s\n", path, strerror(errno));
		destroy_snapshot(ss);
		return 0;
	}

	if (fread(&checksum_read, sizeof(checksum_read), 1, f) < 1) {
		PERR("Cannot read a checksum from the snapshot file %s: %s\n", path, strerror(errno));
		destroy_snapshot(ss);
		return 0;
	}

	// it was stored in a network order
	if (htonl(checksum) != checksum_read) {
		PERR("The snapshot file %s is corrupted (checksum failed): %s\n", path, strerror(errno));
		destroy_snapshot(ss);
		return 0;
	}

	if (fclose(f) != 0) {
		PERR("Cannot close %s: %s\n", path, strerror(errno));
		destroy_snapshot(ss);
		return 0;
	}

	if (!res) {
		destroy_snapshot(ss);
	}

	return res;
}

int pack_snapshot(FILE* f, SNAPSHOT* ss, checksum_t* checksum_p) {
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
		update_checksum(&h, sizeof(h), checksum_p);

		ssentry = cur_hh->first;
		do {
			if (fwrite(ssentry, sizeof(SSENTRY) + ssentry->pathmem, 1, f) < 1) {
				return 0;
			}
			update_checksum(ssentry, sizeof(SSENTRY) + ssentry->pathmem, checksum_p);

			ssentry = ssentry->next;
		} while (ssentry != NULL);
	}

	return 1;
}

int unpack_snapshot(FILE* f, SNAPSHOT* ss, checksum_t* checksum_p) {
	PACK_HASH_HEADER h;
	SSENTRY* ssentry = NULL;
	SSHASH_HEADER* cur_hh = NULL;
	unsigned char* max_next = NULL;

	// iterating over the hash list
	while (fread(&h, sizeof(h), 1, f) == 1) {
		update_checksum(&h, sizeof(h), checksum_p);

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
		update_checksum(cur_hh->first, h.size, checksum_p);

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
		PERR("Cannot read from a snapshot file: %s\n", strerror(errno));
		return 0;
	}

	return 1;
}
