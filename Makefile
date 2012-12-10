dripbix: dripbix.c ssdiff.c snapshot.c tools.c md5.c
	gcc dripbix.c ssdiff.c snapshot.c tools.c md5.c -o dripbix

test: test.c hash.c
	gcc test.c tools.c md5.c -o test

watcher: watcher.c
	gcc watcher.c -o watcher
