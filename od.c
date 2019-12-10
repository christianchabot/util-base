#include <stdlib.h>
#include <string.h>
#include <sys/uio.h>

#include "io.h"

#define FLAGSSTR " [-bcdosvx] [-A [d | o | n | x]] [-j skip] [-N count] [-t type_string] [file...]\n"

enum {AFLAG, bFLAG, cFLAG, dFLAG, jFLAG, NFLAG, oFLAG, sFLAG, tFLAG, vFLAG, xFLAG, FLAGSIZE};
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



static long
xstrtol(const char *str, int base)
{
	long r;

	r = strtol(str, NULL, 0);
	if (!r && errno == ERANGE) {
		
	}
}

static long
offset(const char *str)
{
	size_t len = strlen(str);
	long r;

	if (strchr("bkm", str[len-1]))
		if (strtol(str, NULL, 0)) {
			
		}
}

int
main(int argc, char *argv[])
{
	long r;

	ARGBEG("A:bcdj:N:ost:vx") {
	case 'A':
		if (strlen(optarg) != 1 || !strchr("donx", *optarg))
			usage();
		else
			r = strtol(optarg, NULL, 0);
			if (!r && errno == ERANGE)
				err_exit(1);
		break;
	case 'b':
		break;
	case 'c':
		break;
	case 'd':
		break;
	case 'j':
		break;
	case 'N':
		break;
	case 'o':
		break;
	case 's':
		break;
	case 't':
		break;
	case 'v':
		break;
	case 'x':
		break;
	default:
		usage();
	} ARGEND

	_exit(0);
}
