dripbix: dripbix.c ssdiff.c snapshot.c file.c hash.c
	gcc dripbix.c ssdiff.c snapshot.c file.c hash.c -o dripbix -lssl

test: test.c hash.c file.c
	gcc test.c ssdiff.c snapshot.c hash.c file.c -o test -lssl

watcher: watcher.c
	gcc watcher.c -o watcher
