<?:include	bbb.p ?>
<?:prefix	@_ bbb_util_hash_ ?>
<?:prefix	@^ BBB_UTIL_HASH_ ?>

#ifndef @^H
#define @^H

#include <openssl/sha.h>

#include "bbb.h"

// ================= Exported functions ==================

				// SDBM hashing algorithm
uint32_t		@_Calc_uint32( const void* const buf, const size_t len );

				// SDBM hashing algorithm
				// the result of this is equivalent to ( uint32_hash( buf, len ) & 0x0000FFFF ) 
uint16_t		@_Calc_uint16( const void* const buf, const size_t len );

				// Streaming version of SDBM-32 hashing.
				// Accepts and updates hash value via parameters.
				// The initial value has to be 0.
void			@_UpdateChecksum( const void* const buf, const size_t len, bbb_checksum_t* const checksum );

				// SHA256 of a file ( via OpenSSL )
bbb_result_t	@_ReadFile_sha256( const char* const path, bbb_byte_t hash[ SHA256_DIGEST_LENGTH ] );

#endif
