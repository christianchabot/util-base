#include <string.h>

#include "io.h"

/* NOTE: expand should exit(1) and write error if there is an error accessing one of the files. */

#define FLAGSSTR " [-t tablist] [file...]\n"

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
	int ret = 0;

	ARGBEG("t:") {
	case 't':
		break;
	default:
		usage();
	} ARGEND

	for (; *argv; ++argv) {

	}

	_exit(ret);
}
