var
s1 = {
	"1" : "abc2",
	"2" : "zxcvb",
	"3" : "abc",
	"9" : "tutta"
},

s2 = {
	"1" : "abc",
	"2" : "zxcvb",
	"3" : "t",
	"4" : "abc",
	"5" : "xxx"
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
		protect = [],
		copy_move = {},
		copy = {},
		move = {},
		rsync = {},
		k;

	// Create actions are at the beginning, because they are not mutating data.

	for (k in B) {
		contentLinks[k] = searchContentLinks(B[k], A);

		if (A.hasOwnProperty(k)) {
			if (A[k] === B[k]) {
				protect.push(k);
			} else {
				if (contentLinks[k].length > 0) {
					copy_move[k] = {
						'backup': 1
					};
				} else {
					protect.push(k);
					rsync[k] = {
						'src_content': A[k],
						'dst_content': B[k]
					};
				}
			}
		} else {
			if (contentLinks[k].length > 0) {
				copy_move[k] = {
					'backup': 0
				};
			} else {
				patch.push({
					'type': 'create',
					'key': k,
					'dst_content': B[k]
				});
			}
		}
	}

// TODO rsync protect (3 -> 1) does not work...
// TODO do not modify dst of copy_move (backup)

	// here 'k' is the destination key
	for (k in copy_move) {
		var can = arrayDiff(contentLinks[k], protect);
		if (can.length > 0) {
			protect.push(can[0]);
			move[can[0]] = {
				'to': k
			};
		} else {
			copy[contentLinks[k][0]] = {
				'to': k
			};
		}
	}

	for (k in copy) {
		patch.push({
			'type': 'copy',
			'key': k,
			'to': copy[k].to
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

	return patch;
}

function patch(A, P) {
	return 'nothing...';
}

var d = diff(s1, s2);

console.log(d);

console.log(patch(s1, d));
