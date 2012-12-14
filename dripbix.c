#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "snapshot.h"
#include "pack.h"

// TODO treat DIRs and EMPTYs correctly
// TODO UTF?

int main(int argc, char* argv[]) {
	snapshot_t ss = NULL;
	snapshot_t ss2 = NULL;
	SSENTRY* ssentry = NULL;

	//ss = generate_snapshot("/home/d-ash/2IOMEGA/");
	ss = generate_snapshot("/home/d-ash/distr");
	//ss = generate_snapshot("/home/d-ash/distr/keepassx-0.4.3");


	printf("\nSearching...");
	ssentry = search("/home/d-ash/distr/keepassx-0.4.3.tar.gz", ss);
	if (ssentry == NULL) {
		printf("Not found\n");
	} else {
		printf("Found: %0x\t%s\n", ssentry, SSENTRY_PATH(ssentry));
	}

	printf("\nSearching...");
	ssentry = search("/home/d-ash/distr/keepassx-0.4.3/share/keepassx/i18n/qt_tr.qm", ss);
	if (ssentry == NULL) {
		printf("Not found\n");
	} else {
		printf("Found: %0x\t%s\n", ssentry, SSENTRY_PATH(ssentry));
	}

	printf("\nPacking...");
	printf("%d\n", save_snapshot(ss, "./_packfile"));

	printf("\nUnpacking...");
	ss2 = load_snapshot("./_packfile");
	printf("%d\n", ss2);

	/*
	printf("\nPacking_2...");
	printf("%d\n", save_snapshot(ss2, "./_packfile_2"));
	*/

	destroy_snapshot(ss2);
	destroy_snapshot(ss);

	return 0;
}
