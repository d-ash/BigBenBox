#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "snapshot.h"


int main(int argc, char* argv[]) {
	SNAPSHOT* ss1 = NULL;
	SNAPSHOT* ss2 = NULL;
	SSENTRY* ssentry = NULL;
	unsigned char key[MD5_DIGEST_LENGTH] =
		{ 0x53, 0x60, 0x4d, 0x2e, 0x37, 0x73, 0xd6, 0x2e,
		0x90, 0xfa, 0x5a, 0x34, 0x0a, 0x36, 0x2e, 0x08 };
	

	ss1 = create_snapshot("test1");
	//output_snapshot(ss1);

	ss2 = create_snapshot("/home/d-ash/distr");
	//output_snapshot(ss2);

	printf("\nSearching...");
	ssentry = search_by_path("/home/d-ash/distr/keepassx-0.4.3.tar.gz", ss2);
	printf("Found: %0x\t%s\n", ssentry, ssentry->path);

	printf("\nSearching...");
	ssentry = search_by_path("/home/d-ash/distr/keepassx-0.4.3/share/keepassx/i18n/qt_tr.qm", ss2);
	printf("Found: %0x\t%s\n", ssentry, ssentry->path);

	printf("\nSearching...");
	ssentry = search_by_hash(key, ss2);
	printf("Found: %0x\t%s\n", ssentry, ssentry->path);

	destroy_snapshot(ss1);
	destroy_snapshot(ss2);

	return 0;
}
