#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "snapshot.h"

// TODO treat DIRs and EMPTYs correctly
// TODO UTF?

int main(int argc, char* argv[]) {
	SNAPSHOT* ss1 = NULL;
	SNAPSHOT* ss2 = NULL;
	SSENTRY* ssentry = NULL;

	//ss2 = create_snapshot("/home/d-ash/2IOMEGA/");
	ss2 = create_snapshot("/home/d-ash/distr");
	//ss2 = create_snapshot("/home/d-ash/distr/keepassx-0.4.3");


	printf("\nSearching...");
	ssentry = search("/home/d-ash/distr/keepassx-0.4.3.tar.gz", ss2);
	if (ssentry == NULL) {
		printf("Not found\n");
	} else {
		printf("Found: %0x\t%s\n", ssentry, SSENTRY_PATH(ssentry));
	}

	printf("\nSearching...");
	ssentry = search("/home/d-ash/distr/keepassx-0.4.3/share/keepassx/i18n/qt_tr.qm", ss2);
	if (ssentry == NULL) {
		printf("Not found\n");
	} else {
		printf("Found: %0x\t%s\n", ssentry, SSENTRY_PATH(ssentry));
	}

	printf("\nPacking...");
	printf("%d\n", save_snapshot(ss2, "./_packfile"));

	destroy_snapshot(ss2);

	return 0;
}
