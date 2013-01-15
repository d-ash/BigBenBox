#ifndef _TOOLS_H
#define _TOOLS_H

#include "global.def"

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/types.h>
#include <openssl/sha.h>

typedef uint32_t	checksum_t;

const uint32_t		uint32_hash( const void* const buf, const size_t len );
const uint16_t		uint16_hash( const void* const buf, const size_t len );
void				update_checksum( const void* const buf, const size_t len, checksum_t* checksum );
void				print_hex( const void* const b, const size_t len );
const int			sha256_file( const char* path, unsigned char hash[ SHA256_DIGEST_LENGTH ] );

#endif	// _TOOLS_H
