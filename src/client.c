#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "snapshot.h"
#include "pack.h"

#define DATA_DIR	"../../data"

// TODO treat DIRs and EMPTYs correctly
// TODO UTF?

int main(int argc, char* argv[]) {
	bbbSnapshot_t ss0;
	bbbSnapshot_t ss1;

	bbbTakeSnapshot(DATA_DIR "/tmp0", &ss0);
	bbbTakeSnapshot(DATA_DIR "/tmp1", &ss1);

	/*
	printf("\nSearching...");
	ssentry = bbbSearchSnapshot("/home/d-ash/distr/keepassx-0.4.3.tar.gz", &ss);
	if (ssentry == NULL) {
		printf("Not found\n");
	} else {
		printf("Found: %0x\t%s\n", ssentry, SSENTRY_PATH(ssentry));
	}

	printf("\nSearching...");
	ssentry = bbbSearchSnapshot("/home/d-ash/distr/keepassx-0.4.3/share/keepassx/i18n/qt_tr.qm", &ss);
	if (ssentry == NULL) {
		printf("Not found\n");
	} else {
		printf("Found: %0x\t%s\n", ssentry, SSENTRY_PATH(ssentry));
	}
	*/

	printf("Getting changes...\n");
	printf("%d\n", bbbDiffSnapshot(&ss0, &ss1));

	bbbDestroySnapshot(&ss1);
	bbbDestroySnapshot(&ss0);

	return 0;
}
