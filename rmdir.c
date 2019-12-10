#include <libgen.h>
#include <string.h>
#include <unistd.h>

#include "io.h"

#define FLAGSSTR " [-p] dir...\n"

static unsigned pflag;

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

	ARGBEG("p") {
	case 'p':
		pflag = 1;
		break;
	default:
		usage();
	} ARGEND

	if (!(argc-optind))
		usage();

	for (; *argv; ++argv) {
		char *next = *argv;
		do {
			if (rmdir(next) == -1) {
				file_error_exit(next);
				ret = 1;
				break;
			}
		} while (pflag && strcmp(next = dirname(next), "/") && strcmp(next, "."));
	}

	_exit(ret);
}
