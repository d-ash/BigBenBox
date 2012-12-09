#include "ssdiff.h"

SSPATCH* ssdiff(SNAPSHOT* s1, SNAPSHOT* s2) {
	SSPATCH* patch = NULL;

	patch = malloc(sizeof(SSPATCH));
	patch->actions = NULL;

	return patch;
}
