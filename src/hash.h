#ifndef _BBB_HASH_H
#define _BBB_HASH_H

#include <stdint.h>
#include <openssl/sha.h>

#include "global.h"

typedef uint32_t	bbbChecksum_t;

// ================= Exported functions ==================

			// SDBM hashing algorithm
uint32_t	BbbHashBuf_uint32( const void* const buf, const size_t len );

			// SDBM hashing algorithm
			// the result of this is equivalent to ( uint32_hash( buf, len ) & 0x0000FFFF ) 
uint16_t	BbbHashBuf_uint16( const void* const buf, const size_t len );

			// Streaming version of SDBM-32 hashing.
			// Accepts and returns hash value via parameters.
			// The initial value has to be 0.
void		BbbUpdateChecksum( const void* const buf, const size_t len, bbbChecksum_t* checksum );

int			BbbHashFile_sha256( const char* path, unsigned char hash[ SHA256_DIGEST_LENGTH ] );

#endif
