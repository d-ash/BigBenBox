#include "bigbenbox.h"

#define DATA_DIR	"../../data"

// TODO treat DIRs and EMPTYs correctly
// TODO UTF?

int main( int argc, char* argv[] ) {
	bbbSnapshot_t	ss0;
	bbbSnapshot_t	ss1;

	BbbTakeSnapshot( DATA_DIR "/tmp0", &ss0 );
	BbbTakeSnapshot( DATA_DIR "/tmp1", &ss1 );

	/*
	printf( "\nSearching..." );
	ssentry = BbbSearchSnapshot( "/home/d-ash/distr/keepassx-0.4.3.tar.gz", &ss );
	if ( ssentry == NULL ) {
		printf( "Not found\n" );
	} else {
		printf( "Found: %0x\t%s\n", ssentry, SSENTRY_PATH( ssentry ) );
	}

	printf( "\nSearching..." );
	ssentry = BbbSearchSnapshot( "/home/d-ash/distr/keepassx-0.4.3/share/keepassx/i18n/qt_tr.qm", &ss );
	if ( ssentry == NULL ) {
		printf( "Not found\n" );
	} else {
		printf( "Found: %0x\t%s\n", ssentry, SSENTRY_PATH( ssentry ) );
	}
	*/

	printf( "Getting changes...\n" );
	printf( "%d\n", BbbDiffSnapshot( &ss0, &ss1 ) );

	BbbDestroySnapshot( &ss1 );
	BbbDestroySnapshot( &ss0 );

	return 0;
}
