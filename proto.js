var
s1 = {
	"1" : "abc2",
	"2" : "zxcvb",
	"3" : "abc",
	"9" : "tutta",
	"11": "xxx",
	"555": "need",
	"777": "help"
},

s2 = {
	"1" : "abc",
	"2" : "zxcvb",
	"3" : "t",
	"4" : "abc",
	"5" : "xxx",
	"10": "tutta",
	"11": "abc",
	"12": "nice",
	"555": "help",
	"777": "222222"
};

function addLink(b, a) {
	if (!contentLinks.hasOwnProperty(b)) {
		contentLinks[b] = [];
	}
	contentLinks[b].push(a);
}

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

function diff(A, B) {
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
	return 'nothing...';
}

var d = diff(s1, s2);

console.log(s1);
console.log(s2);
console.log(d);

//console.log(patch(s1, d));

