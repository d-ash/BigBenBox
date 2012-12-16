#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "snapshot.h"
#include "pack.h"

// TODO treat DIRs and EMPTYs correctly
// TODO UTF?

int main(int argc, char* argv[]) {
	SNAPSHOT ss;
	SNAPSHOT ss2;
	SSENTRY* ssentry = NULL;

#ifdef PLATFORM_WINDOWS
	//take_snapshot("C:/Windows", &ss);
	take_snapshot("G:/English", &ss);
#else
	take_snapshot("/home/d-ash/distr", &ss);
	//take_snapshot("/home/d-ash/2IOMEGA/", &ss);
#endif // PLATFORM_WINDOWS

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

	printf("\nPacking...");
	printf("%d\n", save_snapshot("./_packfile", &ss));

	printf("\nUnpacking...");
	printf("%d\n", load_snapshot("./_packfile", &ss2));

	destroy_snapshot(&ss2);
	destroy_snapshot(&ss);

	return 0;
}
