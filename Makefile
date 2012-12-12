dripbix: dripbix.c \
	snapshot.c snapshot.h \
	pack.c pack.h \
	morph.c morph.h \
	md5.c md5.h \
	tools.c tools.h
	gcc -o dripbix dripbix.c snapshot.c pack.c morph.c md5.c tools.c

test: test.c minunit.h \
	tools.c tools.h
	gcc test.c tools.c md5.c -o test

watcher: watcher.c
	gcc watcher.c -o watcher
