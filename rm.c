#include <string.h>
#include <sys/uio.h>

#include "io.h"

#define FLAGSSTR " [-fir] file...\n"

enum {fFLAG, iFLAG, rFLAG, FLAGSIZE};
static unsigned flags[FLAGSIZE];

static void
usage(void)
{
	USAGEBEG {
		USAGESTR, STRLEN(USAGESTR),
		argv0, strlen(argv0),
		FLAGSSTR, STRLEN(FLAGSSTR)
	} USAGEEND
}

int
main(int argc, char *argv[])
{
	ARGBEG("fiRr") {
	case 'f':
		flags[fFLAG] = 1;
		break;
	case 'i':
		flags[iFLAG] = 1;
		break;
	case 'R':
	case 'r':
		flags[rFLAG] = 1;
		break;
	default:
		usage();
	} ARGEND

	for (; *argv; ++argv) {
		
	}

	_exit(0);
}
