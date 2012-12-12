function arrayDiff(a, b) {
	return a.filter(function (i) {
		return (b.indexOf(i) === -1);
	});
}

function arrayCommon(a, b) {
	return a.filter(function (i) {
		return (b.indexOf(i) > -1);
	});
}

function searchContentLinks(content, A) {
	var links = [];

	for (var a in A) {
		if (A[a] === content) {
			links.push(a);
		}
	}

	return links;
}

function morph(A, B) {
	var patch = [],
		contentLinks = {},
		protect = [],		// items protected from moving
		copy_move = {},		// indexes are the destination keys
		overwrites = [],	// these entities will be overwritten
		copy = [],			// one source can be copied several times
		move = {},
		copied = [],
		moved = [],
		rsync = {},
		k, i, can;

	// 'create' actions are at the beginning, because they does not mutate data.

	for (k in B) {
		contentLinks[k] = searchContentLinks(B[k], A);

		if (A.hasOwnProperty(k)) {

			// key existed

			if (A[k] === B[k]) {

				// key and content are the same

				protect.push(k);
			} else {

				// content under this key is different

				if (contentLinks[k].length > 0) {

					// key is the same and the content exists under another key

					overwrites.push(k);		// this action will overwrite existing entity
					copy_move[k] = {
						'backup': 1		// TODO remove
					};
				} else {

					// new content

					protect.push(k);
					rsync[k] = {
						'src_content': A[k],
						'dst_content': B[k]
					};
				}
			}
		} else {

			// new key

			if (contentLinks[k].length > 0) {

				// key is new and the content exists under another key

				copy_move[k] = {
					'backup': 0		// TODO remove
				};
			} else {

				// new key and new content

				patch.push({
					'type': 'create',
					'key': k,
					'dst_content': B[k]
				});
			}
		}
	}

	for (k in copy_move) {

		// here 'k' is the destination key
		// searching where the content can be taken from,
		// moving it if it's possible (faster than coping)

		can = arrayDiff(contentLinks[k], protect);
		if (can.length > 0) {
			protect.push(can[0]);	// don't try to move the source again
			moved.push(can[0]);
			move[can[0]] = {
				'to': k
			};
		} else {
			// TODO check if at least one element is present in contentLinks[k]
			copied.push(contentLinks[k][0]);
			copy.push({
				'key': contentLinks[k][0],
				'to': k
			});
		}
	}

	// treat valuable overwrites only
	overwrites = arrayCommon(overwrites, copied.concat(moved));
	for (i = 0; i < overwrites.length; i += 1) {
		patch.push({
			'type': 'backup',
			'key': overwrites[i]
		});
	}

	// one source can be copied several times,
	// so we use an array for that
	for (i = 0; i < copy.length; i += 1) {
		patch.push({
			'type': 'copy',
			'key': copy[i].key,
			'to': copy[i].to
		});
	}

	for (k in move) {
		patch.push({
			'type': 'move',
			'key': k,
			'to': move[k].to
		});
	}

	for (k in rsync) {
		patch.push({
			'type': 'rsync',
			'key': k,
			'src_content': rsync[k].src_content,
			'dst_content': rsync[k].dst_content
		});
	}

	for (k in A) {
		if (!B.hasOwnProperty(k) && moved.indexOf(k) === -1) {
			patch.push({
				'type': 'delete',
				'key': k
			});
		}
	}

	return patch;
}

function patch(A, P) {
	var act,
		i,
		backup = {},
		storage = {};

	// initially all contents are stored in the source
	for (i = 0; i < P.length; i += 1) {
		storage[P[i].key] = A;
	}

	for (i = 0; i < P.length; i += 1) {
		act = P[i];
		switch (act.type) {
			case 'create':
				if (A.hasOwnProperty(act.key)) {
					console.log("Error: key exists already (create).");
					process.exit(1);
				}
				A[act.key] = act.dst_content;
				break;
			case 'backup':
				if (!A.hasOwnProperty(act.key)) {
					console.log("Error: key does not exist (backup).");
					process.exit(1);
				}
				backup[act.key] = A[act.key];
				storage[act.key] = backup;		// storage for this content is changed
				break;
			case 'copy':
				if (!A.hasOwnProperty(act.key)) {
					console.log("Error: key does not exist (copy).");
					process.exit(1);
				}
				A[act.to] = storage[act.key][act.key];
				break;
			case 'move':
				if (!A.hasOwnProperty(act.key)) {
					console.log("Error: key does not exist (move).");
					process.exit(1);
				}
				A[act.to] = storage[act.key][act.key];
				delete storage[act.key][act.key];	// delete link from the storage (can be backup!)
				break;
			case 'rsync':
				if (A[act.key] !== act.src_content) {
					console.log("Error: src_content is not equal (rsync).");
					process.exit(1);
				}
				A[act.key] = act.dst_content;
				break;
			case 'delete':
				if (!A.hasOwnProperty(act.key)) {
					console.log("Error: key does not exist (delete).");
					process.exit(1);
				}
				delete A[act.key];
				break;
			default:
				console.log("Error: unknown action type.");
				process.exit(1);
				break;
		}
	}

	return A;
}

function is_equal(A, B) {
	var k;

	for (k in A) {
		if (!B.hasOwnProperty(k)) {
			return false;
		}
		if (B[k] !== A[k]) {
			return false;
		}
	}

	for (k in B) {
		if (!A.hasOwnProperty(k)) {
			return false;
		}
	}

	return true;
}

function rnd(min, max) {
	return Math.floor((Math.random() * max) + min);
}

function randSS() {
	var s = {},
		n = rnd(1, 1000);	// number of entities

	while (n > 0) {
		s[n] = rnd(1, 100);
		n -= 1;
	}

	return s;
}

var i,
	s1len = 0,
	s2len = 0,
	dlen = 0,
	s1total = 0,
	s2total = 0,
	dtotal = 0;

for (i = 0; i < 100; i += 1) {
	s1 = randSS();
	s2 = randSS();
	d = morph(s1, s2);

	s1len = Object.keys(s1).length;
	s2len = Object.keys(s2).length;
	dlen = Object.keys(d).length;
	s1total += s1len;
	s2total += s2len;
	dtotal += dlen;
	console.log(s1len + "\t" + s2len + "\t" + dlen);

	patch(s1, d);
	if (!is_equal(s1, s2)) {
		console.log("====== FUCKUP ======");
		process.exit(1);
	}
}

console.log("\n");
console.log("s1total:\t" + s1total / i);
console.log("s2total:\t" + s2total / i);
console.log("dtotal:\t\t" + dtotal / i);
console.log("NICE!");
