#include "test_util.h"
#include "util.h"
#include "util_hash.h"

static int	_testsRun = 0;

static char* _TestHashing() {
	uint32_t	x;
	uint16_t	y;

	x = bbb_util_hash_Calc_uint32( "abcde", 5 );
	y = bbb_util_hash_Calc_uint16( "abcde", 5 );

	MU_ASSERT( "bbb_util_hash_Calc_uint32() failed", x == 0xbd500063 );
	MU_ASSERT( "bbb_util_hash_Calc_uint16() failed", y == 0x0063 );
	return 0;
}

static char* _TestChecksum() {
	const char		s[ 17 ] = "hnd872kz09_2=s9r";
	bbb_checksum_t	x = 0;
	bbb_checksum_t	y = 0;

	bbb_util_hash_UpdateChecksum( s, 16, &x );
	bbb_util_hash_UpdateChecksum( s, 8, &y );
	bbb_util_hash_UpdateChecksum( s + 8, 8, &y );

	MU_ASSERT( "Checksum algorithm failed", x == y );
	return 0;
}

static char* _TestStrncmp() {
	// strncmp()'s third parameter must be ( strlen() + 1 )
	MU_ASSERT( "strncmp() failed (1)", strncmp( "abcdef", "abcde", 5 ) == 0 );
	MU_ASSERT( "strncmp() failed (2)", strncmp( "abcdef", "abcde", 6 ) != 0 );
	return 0;
}

static char* _TestConvertBinary64() {
	const uint64_t x = 0x0102030405060708;
	const uint64_t y = 0x0807060504030201;

	if ( bbb_util_IsLittleEndian() ) {
		printf( "Little-endian platform\n" );
		MU_ASSERT( "_ConvertBinary_hton64() failed", bbb_util_ConvertBinary_hton64( x ) == y );
		MU_ASSERT( "_ConvertBinary_ntoh64() failed", bbb_util_ConvertBinary_ntoh64( y ) == x );
	} else {
		printf( "Big-endian platform\n" );
		MU_ASSERT( "_ConvertBinary_hton64() failed", bbb_util_ConvertBinary_hton64( x ) == x );
		MU_ASSERT( "_ConvertBinary_ntoh64() failed", bbb_util_ConvertBinary_ntoh64( y ) == y );
	}

	return 0;
}

static char* _TestSha256() {
	bbb_byte_t			hash[ SHA256_DIGEST_LENGTH ];
	const bbb_byte_t	ctrl[ SHA256_DIGEST_LENGTH ] = {
		0xcc, 0xfa, 0xf6, 0xdb, 0xae, 0x74, 0x8e, 0xa5,
		0x4e, 0x16, 0x32, 0x2b, 0xcd, 0x04, 0xeb, 0x13,
		0x75, 0x08, 0x3e, 0x65, 0x11, 0x28, 0xae, 0x0c,
		0xe0, 0x7d, 0xdb, 0x6d, 0xce, 0x4f, 0x6e, 0xeb
	};
	
	MU_ASSERT( "Cannot calc SHA256 on a file", bbb_util_hash_ReadFile_sha256( DATA_DIR "/tmp0/sha.hash", hash ) != 0 );
	MU_ASSERT( "SHA256 do not match", memcmp( hash, ctrl, SHA256_DIGEST_LENGTH ) == 0 );
	return 0;
}

// ================================================

static char* _AllTests() {
	MU_RUN_TEST( _TestHashing );
	MU_RUN_TEST( _TestChecksum );
	MU_RUN_TEST( _TestStrncmp );
	MU_RUN_TEST( _TestConvertBinary64 );
	MU_RUN_TEST( _TestSha256 );
	return 0;
}

MAIN
