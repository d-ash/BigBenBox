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
	SNAPSHOT ss0;
	SNAPSHOT ss1;

	take_snapshot(DATA_DIR "/tmp0", &ss0);
	take_snapshot(DATA_DIR "/tmp1", &ss1);

	/*
	printf("\nSearching...");
	ssentry = search("/home/d-ash/distr/keepassx-0.4.3.tar.gz", &ss);
	if (ssentry == NULL) {
		printf("Not found\n");
	} else {
		printf("Found: %0x\t%s\n", ssentry, SSENTRY_PATH(ssentry));
	}

	printf("\nSearching...");
	ssentry = search("/home/d-ash/distr/keepassx-0.4.3/share/keepassx/i18n/qt_tr.qm", &ss);
	if (ssentry == NULL) {
		printf("Not found\n");
	} else {
		printf("Found: %0x\t%s\n", ssentry, SSENTRY_PATH(ssentry));
	}
	*/

	printf("Getting changes...\n");
	printf("%d\n", find_changes(&ss0, &ss1));

	destroy_snapshot(&ss1);
	destroy_snapshot(&ss0);

	return 0;
}
