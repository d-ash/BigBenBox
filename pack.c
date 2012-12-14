#include <sys/stat.h>
#include "pack.h"

pack_snapshot() {
}

int save_snapshot(SNAPSHOT* ss, char* path) {
	FILE* f;
	PACKFILE_HEADER phf;

	f = fopen(path, "w");
	if (f == NULL) {
		PERR("Cannot write a snapshot to %s: %s\n", path, strerror(errno));
		return 0;
	}

	phf.magic = PACKFILE_MAGIC;
	phf.runtime = is_little_endian() ? PACKFILE_LITTLE_END : 0;
#ifdef BITS32
	phf.runtime |= PACKFILE_32_BITS;
#else
#ifdef BITS64
	phf.runtime |= PACKFILE_64_BITS;
#else
#error "BITS.. is not defined"
#endif
#endif
	phf.platform = htons(PLATFORM);
	phf.version = htons(VERSION);
	phf.reserved1 = 0;	// htonl()
	phf.reserved2 = 0;	// htonl()
	phf.reserved3 = 0;	// htons()

	// WARNING: do not write the struct,
	// there is memory packing and it is not portable!
	fwrite(&phf.magic, 1, 1, f);
	fwrite(&phf.runtime, 1, 1, f);
	fwrite(&phf.platform, sizeof(phf.platform), 1, f);
	fwrite(&phf.version, sizeof(phf.version), 1, f);
	fwrite(&phf.reserved1, sizeof(phf.reserved1), 1, f);
	fwrite(&phf.reserved2, sizeof(phf.reserved2), 1, f);
	fwrite(&phf.reserved3, sizeof(phf.reserved3), 1, f);
	
	/*
	unsigned char* data = NULL;
	SSENTRY* ssentry = NULL;
	unsigned char* ptr = NULL;
	size_t chunk = 0;

	data = malloc(ss->entries_count * sizeof(SSENTRY_HEADER) + ss->pathmem_total);
	ptr = data;

	ht = ss->ht_by_hash;
	while (ht != NULL) {
		ssentry = ht->entries;
		while (ssentry != NULL) {
			chunk = sizeof(SSENTRY_HEADER) + ssentry->header.pathmem;
			memcpy(ptr, &ssentry->header, chunk);	// coping header and path
			ptr += chunk;
			ssentry = ssentry->next_by_hash;
		}
		ht = ht->next;
	}
	*/

	if (fclose(f) != 0) {
		PERR("Cannot save a snapshot to %s: %s\n", path, strerror(errno));
		return 0;
	}

	return 1;
}

SNAPSHOT* unpack_snapshot(char* path, unsigned char* data, off_t len) {
	unsigned char* ptr;
	SNAPSHOT* ss = NULL;
	PACK_HASH_HEADER* h = NULL;
	SSENTRY* ssentry = NULL;

	if (data[0] != PACKFILE_MAGIC) {
		PERR("Incorrect PACKFILE_MAGIC in %s\n", path);
		return NULL;
	}

// TODO check header values
// TODO check length (reading)

	ptr = data + PACKFILE_HEADER_SIZE;

	ss = malloc(sizeof(SNAPSHOT));
	memset(ss->ht, 0, sizeof(void*) * HASH_MAX);

	// iterating over the hash list
	do {
		h = (PACK_HASH_HEADER*) ptr;
		ptr += sizeof(PACK_HASH_HEADER);

		// allocating memory for all entries with this hash
		ss->ht[h->hash] = malloc(h->size);
		memcpy(ss->ht[h->hash], ptr, h->size);
		ptr += h->size;
		// now 'ptr' points to the next PACK_HASH_HEADER

		// go through all entries, the last one must have ssentry->next == NULL
		ssentry = ss->ht[h->hash];
		do {
			// setting correct values of SSENTRY.next
			// all restored pointers are incorrect,
			// but we are searching for NULL value!
			ssentry->next = ssentry + sizeof(SSENTRY) + ssentry->header.pathmem;
			ssentry = ssentry->next;
		} while (ssentry->next != NULL);
	} while (ptr - data < len);

	return ss;
}

SNAPSHOT* load_snapshot(char* path) {
	int fd;
	char* file_buffer;
	SNAPSHOT* ss;
	struct stat st;

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

	ss = unpack_snapshot(path, file_buffer, st.st_size);

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
