#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/uio.h>

#include "io.h"

#define FLAGSSTR1 " [-Pfip] source_file target_file\n"
#define FLAGSSTR2 " [-Pfip] source_file target...\n"
#define FLAGSSTR3 " -R [-H | -L | -P] [-fip] source_file... target\n"

enum {fFLAG, HFLAG, iFLAG, LFLAG, PFLAG, pFLAG, RFLAG, FLAGSIZE};
static unsigned flags[FLAGSIZE];

static int
cp(void)
{
	struct stat f1, f2;

	
}

static void
usage(void)
{
	USAGEBEG {
		USAGESTR, STRLEN(USAGESTR),
		argv0, strlen(argv0),
		FLAGSSTR1, STRLEN(FLAGSSTR1),
		USAGEALN, STRLEN(USAGEALN),
		argv0, strlen(argv0),
		FLAGSSTR2, STRLEN(FLAGSSTR2),
		USAGEALN, STRLEN(USAGEALN),
		argv0, strlen(argv0),
		FLAGSSTR3, STRLEN(FLAGSSTR3)
	} USAGEEND
}

int
main(int argc, char *argv[])
{
	struct stat target;

	ARGBEG("fHiLPp") {
	case 'f':
		flags[fFLAG] = 1;
		break;
	case 'H':
		flags[HFLAG] = 1;
		break;
	case 'i':
		flags[iFLAG] = 1;
		break;
	case 'L':
		flags[LFLAG] = 1;
		break;
	case 'P':
		flags[PFLAG] = 1;
		break;
	case 'p':
		flags[pFLAG] = 1;
		break;
	case 'R':
		flags[RFLAG] = 1;
		break;
	default:
		usage();
	} ARGEND

	if (argc-optind < 2)
		usage();

	/* get target first */
	if (stat(argv[argc-optind-1], &target) == -1) {
		if (errno == EEXIST) {

		}
	}

	_exit(0);
}
