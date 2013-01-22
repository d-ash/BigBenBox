#include "test_util_bio.h"
#include "util_bio.h"

static int	_testsRun = 0;

static char* _TestBioTypes() {
	static const char		path[18] = "test_util_bio.dat";
	static const bbb_byte_t	data[ 20 ] = {
		0x00, 0x00, 0x00, 0x10,		// len of the following seria
		0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
		0x09, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16
	};
	static const uint16_t	c16 = 0x0102;
	static const uint32_t	c32 = 0x03040506;
	static const uint64_t	c64 = 0x0708091011121314;
	bbb_varbuf_t	vb;
	uint16_t		x16;
	uint32_t		x32;
	uint64_t		x64;
	FILE*			f;

	f = fopen( path, "wb" );
	BBB_MU_ASSERT( "Cannot create a data file", f != NULL );
	BBB_MU_ASSERT( "Cannot write data",
		fwrite( data, sizeof( data ), 1, f ) == 1 );
	BBB_MU_ASSERT( "Cannot close a data file", fclose( f ) == 0 );
	f = fopen( path, "rb" );

	fseek( f, 4, SEEK_SET );
	BBB_MU_ASSERT( "Cannot read uint16_t", bbb_util_bio_Read_uint16( &x16, f ) == 1 );
	BBB_MU_ASSERT( "bbb_util_bio_Read_uint16() failed", x16 == c16 );
	BBB_MU_ASSERT( "Cannot read uint32_t", bbb_util_bio_Read_uint32( &x32, f ) == 1 );
	BBB_MU_ASSERT( "bbb_util_bio_Read_uint32() failed", x32 == c32 );
	BBB_MU_ASSERT( "Cannot read uint64_t", bbb_util_bio_Read_uint64( &x64, f ) == 1 );
	BBB_MU_ASSERT( "bbb_util_bio_Read_uint64() failed", x64 == c64 );

	fseek( f, 0, SEEK_SET );
	BBB_MU_ASSERT( "Cannot read varbuf", bbb_util_bio_Read_varbuf( &vb, f ) == 1 );
	if ( vb.len != 16 || memcmp( vb.buf, data + 4, vb.len ) != 0 ) {
		BBB_MU_ASSERT( "bbb_util_bio_Read_varbuf() failed", 0 );
	}

	fclose( f );
	return 0;
}

// ================================================

static char* _AllTests() {
	BBB_MU_RUN_TEST( _TestBioTypes );
	return 0;
}

MAIN
