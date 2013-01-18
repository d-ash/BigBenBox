#include <stdio.h>

#include "unit.h"

#define DATA_DIR	"../../../data"

int		_tests_run = 0;

static char* test_uint16_32_hashes() {
	uint32_t	x;
	uint16_t	y;

	x = bbbHashBuf_uint32( "abcde", 5 );
	y = bbbHashBuf_uint16( "abcde", 5 );

	MU_ASSERT( "bbbHashBuf_uint32() failed", x == 0xbd500063 );
	MU_ASSERT( "bbbHashBuf_uint16() failed", y == 0x0063 );
	return 0;
}

static char* test_checksum() {
	const char		s[ 17 ] = "hnd872kz09_2=s9r";
	bbbChecksum_t	x = 0;
	bbbChecksum_t	y = 0;

	bbbUpdateChecksum( s, 16, &x );
	bbbUpdateChecksum( s, 8, &y );
	bbbUpdateChecksum( s + 8, 8, &y );

	MU_ASSERT( "Checksum algorithm failed", x == y );
	return 0;
}

static char* test_strncmp() {
	// strncmp()'s third parameter must be ( strlen() + 1 )
	MU_ASSERT( "strncmp() failed (1)", strncmp( "abcdef", "abcde", 5 ) == 0 );
	MU_ASSERT( "strncmp() failed (2)", strncmp( "abcdef", "abcde", 6 ) != 0 );
	return 0;
}

static char* test_sha256() {
	unsigned char			hash[ SHA256_DIGEST_LENGTH ];
	const unsigned char		ctrl[ SHA256_DIGEST_LENGTH ] = {
		0xcc, 0xfa, 0xf6, 0xdb, 0xae, 0x74, 0x8e, 0xa5,
		0x4e, 0x16, 0x32, 0x2b, 0xcd, 0x04, 0xeb, 0x13,
		0x75, 0x08, 0x3e, 0x65, 0x11, 0x28, 0xae, 0x0c,
		0xe0, 0x7d, 0xdb, 0x6d, 0xce, 0x4f, 0x6e, 0xeb
	};
	
	MU_ASSERT( "Cannot calc SHA256 on a file", bbbHashFile_sha256( DATA_DIR "/tmp0/sha.hash", hash ) != 0 );
	MU_ASSERT( "SHA256 do not match", memcmp( hash, ctrl, SHA256_DIGEST_LENGTH ) == 0 );
	return 0;
}

static char* test_ssentry_size() {
	printf( "sizeof( size_t ): %lu\n", sizeof( size_t ) );
	printf( "sizeof( bbbSsEntry_t ): %lu\n", sizeof( bbbSsEntry_t ) );

	MU_ASSERT( "sizeof( bbbSsEntry_t ) is not appropriate for manual path aligment.", ( sizeof( bbbSsEntry_t ) % BBB_WORD_SIZE ) == 0 );
	return 0;
}

static char* test_snapshot_save_load() {
	bbbSnapshot_t	ss1;
	bbbSnapshot_t	ss2;

#ifdef BBB_PLATFORM_WINDOWS
	bbbTakeSnapshot( "G:\\English\\", &ss1 );
	//bbbTakeSnapshot( "C:/Windows", &ss1 );
#endif
#ifdef BBB_PLATFORM_LINUX
	bbbTakeSnapshot( "/home/d-ash/Dropbox", &ss1 );
	//bbbTakeSnapshot( "/home/d-ash/2IOMEGA/", &ss1 );
#endif
#ifdef BBB_PLATFORM_OSX
	bbbTakeSnapshot( "/Users/User/Projects/bbb", &ss1 );
	//bbbTakeSnapshot( "/home/d-ash/2IOMEGA/", &ss1 );
#endif

	save_snapshot( "_test_packfile", &ss1 );
	load_snapshot( "_test_packfile", &ss2 );

	MU_ASSERT( "Different values of tf_path", strcmp( ss1.tf_path, ss2.tf_path ) == 0 );
	MU_ASSERT( "Restored snapshot differs from original", bbbDiffSnapshot( &ss1, &ss2 ) == 0 );

	//unlink("_test_packfile");
	bbbDestroySnapshot( &ss2 );
	bbbDestroySnapshot( &ss1 );
	return 0;
}

// ================================================

static char* all_tests() {
	mu_run_test( test_uint16_32_hashes );
	mu_run_test( test_checksum );
	mu_run_test( test_strncmp );
	mu_run_test( test_sha256 );
	mu_run_test( test_ssentry_size );
	mu_run_test( test_snapshot_save_load );
	return 0;
}

int main( const int argc, const char* const argv[] ) {
	char* result;
	
	result = all_tests();

	if ( result == 0 ) {
		printf( "\nALL TESTS PASSED\n" );
	} else {
		printf( "\nTEST FAILED: %s\n", result );
	}
	printf( "Tests run: %d\n", tests_run );

	return ( int ) ( result != 0 );
}
