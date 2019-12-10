#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define FLAGSSTR " [-a] [-t format] [-n number] [file...]\n"

enum {nFLAG, tFLAG, FLAGSIZE};
static unsigned flags[FLAGSIZE];

static char targ;
static size_t narg;

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
	int fd = 0;

	ARGBEG("an:t:") {
	case 'a':
		break;
	case 't':
		if (strcmp(optarg, "d") && strcmp(optarg, "o") && strcmp(optarg, "x"))
			usage();
		targ = optarg[0];
		tFLAG = 1;
		break;
	case 'n':
		narg = strtoul(optarg, NULL, 10);
		if (errno != ERANGE && errno != EINVAL)
			nFLAG = 1;
		errno = 0;
		break;
	} ARGEND;

	for (; *argv; ++argv) {
		
	}

	_exit(0);
}
