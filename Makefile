dripbix: dripbix.c \
	snapshot.c snapshot.h \
	pack.c pack.h \
	morph.c morph.h \
	tools.c tools.h
	gcc -DBITS32 -o dripbix dripbix.c snapshot.c pack.c morph.c tools.c #-DDEBUG 

test: test.c minunit.h \
	tools.c tools.h
	gcc test.c tools.c -o test

watcher: watcher.c
	gcc watcher.c -o watcher
