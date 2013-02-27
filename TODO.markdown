	BBB_RESULT and Exceptions
	BBB_RESULT fname( ... );
	BBB_CALL( fname( ... ) );
		|
		|
		v
	if ( ( _bbb_result = fname( ... ) ) != BBB_SUCCESS ) {
		BBB_PERR( _bbb_result );
		reVal = ...;
		goto cleanup;
	}
	The same with BBB_MALLOC
	BBB_MALLOC( buf, len );
	If the only possible errors are programmer errors, don't return an error code, use asserts inside the function.
	It would be nice if application programmer can set an global error-callback. That way they will be able to put a breakpoint into the callback during bug-hunt sessions.

	For multithreading: There is something called "Thread Local Storage" in Windows and "Thread Specific Storage" in POSIX

	SLA parameters!!!
	Tags (like in Git).
	Command and control synchronization manually (lib, web, CLI).

READ
  https://github.com/joyent/libuv
  using libuv: http://vimeo.com/24713213

Competitors:
 * box.net
 * powerfolder.com (?)
 * filosync.com (?)

Files are complex!
https://www.dropbox.com/help/145/en
+ file permissions.
+ UTF8/16

config (exclude like .gitignore!)
access permissions (LDAP auth)
web-access
file encryption (on server and on clients, current user can be EXCLUDED from the group of using file)
banding and customization
backups
hooks (events), API
data storage deduplication: http://pdos.csail.mit.edu/papers/lbfs:sosp01/lbfs.pdf

TODO:
0. How to treat directories, trees?
1. Universal Makefile (look at git makefile)
2. Instead of zlib use liblzma (http://tukaani.org/xz/)
3. Warn a user when mtime of the file is later than now.

Searching:
1. Zero-step, go up to the first splitting found.
2. The second changing of direction is not allowed.
3. Lockfiles for: head, lastid, stones/.../ch*

Syntax highlighting does not work:
http://stackoverflow.com/questions/5176972/trouble-using-vims-syn-include-and-syn-region-to-embed-syntax-highlighting
