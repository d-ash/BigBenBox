#include "test_util.h"

static int	_testsRun = 0;

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

// ================================================

static char* _AllTests() {
	MU_RUN_TEST( _TestConvertBinary64 );
	return 0;
}

MAIN
