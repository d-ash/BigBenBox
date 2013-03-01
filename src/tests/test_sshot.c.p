#include "test_sshot.h"

static int	_testsRun = 0;

static char* _TestSsEntrySize() {
	printf( "sizeof( size_t ): %" PRIuPTR "\n", sizeof( size_t ) );
	printf( "sizeof( bbb_sshot_entry_t ): %" PRIuPTR "\n", sizeof( bbb_sshot_entry_t ) );

	MU_ASSERT( "sizeof( bbb_sshot_entry_t ) is not appropriate for manual path alignment.",
		( sizeof( bbb_sshot_entry_t ) % BBB_WORD_SIZE ) == 0 );

	return 0;
}

static char* _TestSsSaveLoad() {
	bbb_sshot_t		ss1;
	bbb_sshot_t		ss2;

#ifdef BBB_PLATFORM_WINDOWS
	bbb_sshot_Take( "G:\\English\\", &ss1 );
	//bbb_sshot_Take( "C:/Windows", &ss1 );
#endif
#ifdef BBB_PLATFORM_LINUX
	bbb_sshot_Take( "/home/d-ash/Dropbox", &ss1 );
	//bbb_sshot_Take( "/home/d-ash/2IOMEGA/", &ss1 );
#endif
#ifdef BBB_PLATFORM_OSX
	bbb_sshot_Take( "/Users/User/Projects/bbb", &ss1 );
	//bbb_sshot_Take( "/home/d-ash/2IOMEGA/", &ss1 );
#endif

	MU_ASSERT( "bbb_sshot_file_Save", bbb_sshot_file_Save( "_test_packfile", &ss1 ) );
	MU_ASSERT( "bbb_sshot_file_Load", bbb_sshot_file_Load( "_test_packfile", &ss2 ) );
	MU_ASSERT( "Restored snapshot differs from original", bbb_sshot_Diff( &ss1, &ss2 ) == 0 );
	MU_ASSERT( "Different values of 'takenFrom'", strcmp( ss1.takenFrom, ss2.takenFrom ) == 0 );

	unlink( "_test_packfile" );
	bbb_sshot_Destroy( &ss2 );
	bbb_sshot_Destroy( &ss1 );
	return 0;
}

// ================================================

static char* _AllTests() {
	MU_RUN_TEST( _TestSsEntrySize );
	MU_RUN_TEST( _TestSsSaveLoad );
	return 0;
}

MAIN
