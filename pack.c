#include <sys/stat.h>
#include "pack.h"

int pack_snapshot(snapshot_t ss, FILE* f) {
	hash_t i;
	SSENTRY* ssentry = NULL;
	PACK_HASH_HEADER h;

	for (i = 0; i < HASH_MAX; i++) {
		if (ss[i].first == NULL) {
			// do not store hash with no entries
			continue;
		}

		h.hash = i;
		h.size = ss[i].size;
		fwrite(&h, sizeof(PACK_HASH_HEADER), 1, f);

		ssentry = ss[i].first;
		do {
			fwrite(ssentry, sizeof(SSENTRY) + ssentry->pathmem, 1, f);
			ssentry = ssentry->next;
		} while (ssentry != NULL);
	}
}

snapshot_t unpack_snapshot(unsigned char* data, off_t len) {
	unsigned char* ptr;
	snapshot_t ss = NULL;
	PACK_HASH_HEADER* h = NULL;
	SSENTRY* ssentry = NULL;

	if (data[0] != PACKFILE_MAGIC) {
		PERR("Incorrect PACKFILE_MAGIC.\n");
		return NULL;
	}

// TODO check header values
// TODO check length (reading)
// TODO check hash_header.size

	ptr = data + PACKFILE_HEADER_SIZE;

	ss = create_snapshot();

	// iterating over the hash list
	do {
		h = (PACK_HASH_HEADER*) ptr;
		ptr += sizeof(PACK_HASH_HEADER);

		// allocating memory for all entries with this hash
		ss[h->hash].first = malloc(h->size);
		ss[h->hash].size = h->size;

		memcpy(ss[h->hash].first, ptr, h->size);
		ptr += h->size;
		// now 'ptr' points to the next PACK_HASH_HEADER

		// go through all entries, the last one must have ssentry->next == NULL
		ssentry = ss[h->hash].first;
		while (ssentry->next != NULL) {
			PERR("ssentry = %0X\n", ssentry);
			// set correct values for SSENTRY.next
			// all restored pointers are incorrect,
			// but we are searching for NULL value!
			ssentry->next = (unsigned char*) ssentry + sizeof(SSENTRY) + ssentry->pathmem;
			ssentry = ssentry->next;
		}
	} while (ptr - data < len);

	return ss;
}

int save_snapshot(snapshot_t ss, char* path) {
	FILE* f;
	unsigned char pfh[PACKFILE_HEADER_SIZE];

	f = fopen(path, "w");
	if (f == NULL) {
		PERR("Cannot write a snapshot to %s: %s\n", path, strerror(errno));
		return 0;
	}

	pfh[0] = PACKFILE_MAGIC;
	pfh[1] = is_little_endian() ? PACKFILE_LITTLE_END : 0;
	pfh[1] |= sizeof(size_t);

	*(uint16_t*)(pfh + 2) = htons(PLATFORM);
	*(uint16_t*)(pfh + 4) = htons(VERSION);
	memset(pfh + 6, 0, 10);

	fwrite(&pfh, sizeof(pfh), 1, f);
	
	pack_snapshot(ss, f);
	// TODO check errors

	if (fclose(f) != 0) {
		PERR("Cannot save a snapshot to %s: %s\n", path, strerror(errno));
		return 0;
	}

	return 1;
}

snapshot_t load_snapshot(char* path) {
	int fd;
	char* file_buffer;
	snapshot_t ss = NULL;
	struct stat st;

	// TODO mmap is not portable! Replace with fread/seek...

	fd = open(path, O_RDONLY);
	if (fd < 0) {
		PERR("Cannot read a snapshot from %s: %s\n", path, strerror(errno));
		exit(EXIT_FAILURE);
	}

	if (fstat(fd, &st) != 0) {
		PERR("Cannot fstat() file %s: %s\n", path, strerror(errno));
		exit(EXIT_FAILURE);
	}

    file_buffer = mmap(0, st.st_size, PROT_READ, MAP_SHARED, fd, 0);
	if (file_buffer == MAP_FAILED) {
		PERR("Cannot mmap() file %s (%lld bytes): %s\n", path, (long long) st.st_size, strerror(errno));
		exit(EXIT_FAILURE);
	}

	ss = unpack_snapshot(file_buffer, st.st_size);
	// TODO check errors

	if (munmap(file_buffer, st.st_size) < 0) {
		PERR("Cannot munmap() file %s (%lld bytes): %s\n", path, (long long) st.st_size, strerror(errno));
		exit(EXIT_FAILURE);
	}

	if (close(fd) < 0) {
		PERR("Cannot close file %s: %s\n", path, strerror(errno));
		exit(EXIT_FAILURE);
	}

	return ss;
}
