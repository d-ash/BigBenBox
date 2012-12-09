dripbix: dripbix.c ssdiff.c snapshot.c file.c hash.c
	gcc dripbix.c ssdiff.c snapshot.c file.c hash.c -o dripbix -lssl

test: test.c hash.c
	gcc test.c hash.c -o test -lssl

watcher: watcher.c
	gcc watcher.c -o watcher
