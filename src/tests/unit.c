#include "unit.h"
#include "bigbenbox.h"

#define DATA_DIR	"../../../data"

static int		_testsRun = 0;

static char* _TestHashing() {
	uint32_t	x;
	uint16_t	y;

	x = BbbUtilHashBuf_uint32( "abcde", 5 );
	y = BbbUtilHashBuf_uint16( "abcde", 5 );

	BBB_MU_ASSERT( "BbbUtilHashBuf_uint32() failed", x == 0xbd500063 );
	BBB_MU_ASSERT( "BbbUtilHashBuf_uint16() failed", y == 0x0063 );
	return 0;
}

static char* _TestChecksum() {
	const char		s[ 17 ] = "hnd872kz09_2=s9r";
	bbbChecksum_t	x = 0;
	bbbChecksum_t	y = 0;

	BbbUtilHashChecksum( s, 16, &x );
	BbbUtilHashChecksum( s, 8, &y );
	BbbUtilHashChecksum( s + 8, 8, &y );

	BBB_MU_ASSERT( "Checksum algorithm failed", x == y );
	return 0;
}

static char* _TestStrncmp() {
	// strncmp()'s third parameter must be ( strlen() + 1 )
	BBB_MU_ASSERT( "strncmp() failed (1)", strncmp( "abcdef", "abcde", 5 ) == 0 );
	BBB_MU_ASSERT( "strncmp() failed (2)", strncmp( "abcdef", "abcde", 6 ) != 0 );
	return 0;
}

static char* _TestSha256() {
	bbbByte_t		hash[ SHA256_DIGEST_LENGTH ];
	const bbbByte_t	ctrl[ SHA256_DIGEST_LENGTH ] = {
		0xcc, 0xfa, 0xf6, 0xdb, 0xae, 0x74, 0x8e, 0xa5,
		0x4e, 0x16, 0x32, 0x2b, 0xcd, 0x04, 0xeb, 0x13,
		0x75, 0x08, 0x3e, 0x65, 0x11, 0x28, 0xae, 0x0c,
		0xe0, 0x7d, 0xdb, 0x6d, 0xce, 0x4f, 0x6e, 0xeb
	};
	
	BBB_MU_ASSERT( "Cannot calc SHA256 on a file", BbbUtilHashFile_sha256( DATA_DIR "/tmp0/sha.hash", hash ) != 0 );
	BBB_MU_ASSERT( "SHA256 do not match", memcmp( hash, ctrl, SHA256_DIGEST_LENGTH ) == 0 );
	return 0;
}

static char* _TestSsEntrySize() {
	printf( "sizeof( size_t ): %" PRIuPTR "\n", sizeof( size_t ) );
	printf( "sizeof( bbbSsEntry_t ): %" PRIuPTR "\n", sizeof( bbbSsEntry_t ) );

	BBB_MU_ASSERT( "sizeof( bbbSsEntry_t ) is not appropriate for manual path alignment.", ( sizeof( bbbSsEntry_t ) % BBB_WORD_SIZE ) == 0 );
	return 0;
}

static char* _TestSsSaveLoad() {
	bbbSnapshot_t	ss1;
	bbbSnapshot_t	ss2;

#ifdef BBB_PLATFORM_WINDOWS
	BbbTakeSnapshot( "G:\\English\\", &ss1 );
	//BbbTakeSnapshot( "C:/Windows", &ss1 );
#endif
#ifdef BBB_PLATFORM_LINUX
	BbbTakeSnapshot( "/home/d-ash/Dropbox", &ss1 );
	//BbbTakeSnapshot( "/home/d-ash/2IOMEGA/", &ss1 );
#endif
#ifdef BBB_PLATFORM_OSX
	BbbTakeSnapshot( "/Users/User/Projects/bbb", &ss1 );
	//BbbTakeSnapshot( "/home/d-ash/2IOMEGA/", &ss1 );
#endif

	BbbSaveSnapshot( "_test_packfile", &ss1 );
	BbbLoadSnapshot( "_test_packfile", &ss2 );

	BBB_MU_ASSERT( "Different values of 'takenFrom'", strcmp( ss1.takenFrom, ss2.takenFrom ) == 0 );
	BBB_MU_ASSERT( "Restored snapshot differs from original", BbbDiffSnapshot( &ss1, &ss2 ) == 0 );

	//unlink("_test_packfile");
	BbbDestroySnapshot( &ss2 );
	BbbDestroySnapshot( &ss1 );
	return 0;
}

// ================================================

static char* _AllTests() {
	BBB_MU_RUN_TEST( _TestHashing );
	BBB_MU_RUN_TEST( _TestChecksum );
	BBB_MU_RUN_TEST( _TestStrncmp );
	BBB_MU_RUN_TEST( _TestSha256 );
	BBB_MU_RUN_TEST( _TestSsEntrySize );
	BBB_MU_RUN_TEST( _TestSsSaveLoad );
	return 0;
}

int main( const int argc, const char* const argv[] ) {
	char*	result;
	
	result = _AllTests();

	if ( result == 0 ) {
		printf( "\nALL TESTS PASSED\n" );
	} else {
		printf( "\nTEST FAILED: %s\n", result );
	}
	printf( "Tests run: %d\n", _testsRun );

	return ( int ) ( result != 0 );
}
