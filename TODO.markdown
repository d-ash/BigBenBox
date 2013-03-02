CUT THE FEATURES, SHIP EARLY!
 * clients on Mac, Win, Linux (libuv)
 * server (libuv)
 * no encryption
 * simple web-configuration (MySQL or even text files)
 * simple GUI (little QT-app)


	SLA parameters!!!
	Tags (like in Git).
	Command and control synchronization manually (lib, web, CLI).

READ
  https://github.com/joyent/libuv
  using libuv: http://vimeo.com/24713213

Competitors:
 * box.net (.com, enterprise version)
 * powerfolder.com (?)
 * filosync.com (?)
 * http://www.druva.com/insync/  (?)
 * www.sugarsync.com (?)
 * https://pogoplug.com/business (?)
 * https://www.egnyte.com/corp/ (?)
 * https://oxygencloud.com/
 * http://www.tonido.com/ ( http://www.techrepublic.com/blog/itdojo/tonido-a-dropbox-alternative-for-smbs/3966 )
 * http://www.accellion.com/
 * http://betanews.com/2012/06/30/four-self-hosted-dropbox-like-services-businesses-can-use/
 * http://community.spiceworks.com/topic/260697-anyone-using-a-good-self-hosted-dropbox-alternative?page=2

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
2. Instead of zlib use liblzma (http://tukaani.org/xz/)
3. Warn a user when mtime of the file is later than now.

Searching:
1. Zero-step, go up to the first splitting found.
2. The second changing of direction is not allowed.
3. Lockfiles for: head, lastid, stones/.../ch*
