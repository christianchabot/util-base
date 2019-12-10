#include <string.h>
#include <sys/statvfs.h>
#include <unistd.h>

#include "io.h"

#define FLAGSSTR " [-k] [-P | -t] [file...]\n"

#define MTABNAME "/etc/mtab"

enum {kFLAG, PFLAG, tFLAG, FLAGSIZE};
static unsigned flags[FLAGSIZE];

static int
mtabinit(void)
{
	
}

static int
mtabclose(void)
{

}

static struct statvfs *
parse_mtab(struct statvfs *vfs)
{
	static struct statvfs tmp;

	while (1);

	return NULL;
}

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

	ARGBEG("kPt") {
	case 'k':
		flags[kFLAG] = 1;
		break;
	case 'P':
		flags[PFLAG] = 1;
		break;
	case 't':
		flags[tFLAG] = 1;
		break;
	default:
		usage();
	} ARGEND

	if (flags[PFLAG] + flags[tFLAG] > 1)
		usage();

	if (argc - optind < 1)
		/* print all filesystems using mtab */

	while (*argv) {
		/* print selected filesystems; use mtab to get the fs name */
	}

	_exit(ret);
}
