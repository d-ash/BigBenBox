#include "bigbenbox.h"

#define DATA_DIR	"../../test_data"

// TODO treat DIRs and EMPTYs correctly
// TODO UTF?

int main() {
	bbb_sshot_t	ss0;
	bbb_sshot_t	ss1;

	bbb_sshot_Take( DATA_DIR "/tmp0", &ss0 );
	bbb_sshot_Take( DATA_DIR "/tmp1", &ss1 );

	/*
	printf( "\nSearching..." );
	ssentry = bbb_sshot_Search( "/home/d-ash/distr/keepassx-0.4.3.tar.gz", &ss );
	if ( ssentry == NULL ) {
		printf( "Not found\n" );
	} else {
		printf( "Found: %0x\t%s\n", ssentry, SSENTRY_PATH( ssentry ) );
	}

	printf( "\nSearching..." );
	ssentry = bbb_sshot_Search( "/home/d-ash/distr/keepassx-0.4.3/share/keepassx/i18n/qt_tr.qm", &ss );
	if ( ssentry == NULL ) {
		printf( "Not found\n" );
	} else {
		printf( "Found: %0x\t%s\n", ssentry, SSENTRY_PATH( ssentry ) );
	}
	*/

	printf( "Getting changes...\n" );
	printf( "%d\n", bbb_sshot_Diff( &ss0, &ss1 ) );

	bbb_sshot_Destroy( &ss1 );
	bbb_sshot_Destroy( &ss0 );

	return 0;
}
