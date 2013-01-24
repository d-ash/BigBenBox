TODO FIX: test buffer I/O and destroy already read records when the whole process of reading has failed.

Files are complex!
https://www.dropbox.com/help/145/en
+ file permissions.

config (exclude like .gitignore)
access permissions (LDAP auth)
web-access
file encryption (on server and on clients)
banding and customization
backups
hooks (events), API

TODO:
0. How to treat directories, trees?
1. Universal Makefile (look at git makefile)
2. Instead of zlib use liblzma (http://tukaani.org/xz/)
3. Warn a user when mtime of the file is later than now.

Searching:
1. Zero-step, go up to the first splitting found.
2. The second changing of direction is not allowed.
3. Lockfiles for: head, lastid, stones/.../ch*
