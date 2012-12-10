var
s1 = {
	"1" : "abc2",
	"2" : "zxcvb",
	"3" : "abc"
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
		copy_move = [],
		move = {},
		b;

	for (b in B) {
		contentLinks[b] = searchContentLinks(B[b], A);

		if (A.hasOwnProperty(b)) {
			if (A[b] === B[b]) {
				protect.push(b);
			} else {
				if (contentLinks[b].length > 0) {
					copy_move.push(b);
				} else {
					protect.push(b);
					patch.push({
						'type': 'rsync',
						'key': b,
						'src_content': A[b],
						'dst_content': B[b]
					});
				}
			}
		} else {
			if (contentLinks[b].length > 0) {
				copy_move.push(b);
			} else {
				patch.push({
					'type': 'create',
					'key': b,
					'dst_content': B[b]
				});
			}
		}
	}

	for (var i = 0; i < copy_move.length; i += 1) {
		b = copy_move[i];
		var can = arrayDiff(contentLinks[b], protect);
		if (can.length > 0) {
			protect.push(can[0]);
			patch.push({
				'type': 'move',
				'key': can[0],
				'to': b
			});
		} else {
			patch.push({
				'type': 'copy',
				'key': contentLinks[b][0],
				'to': b
			});
		}
	}

	return patch.reverse();
}

console.log(diff(s1, s2));
