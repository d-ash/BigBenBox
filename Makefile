dripbix: patch.c
	gcc patch.c -g -o dripbix -lssl

watcher: watcher.c
	gcc watcher.c -o watcher
