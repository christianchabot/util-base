#include <string.h>
#include <sys/uio.h>
#include <sys/utsname.h>

#include "io.h"

#define FLAGSSTR " [-amnrsv]\n"

enum {sFLAG, nFLAG, rFLAG, vFLAG, mFLAG, FLAGSIZE};
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
	struct iovec iov[FLAGSIZE*2 + 1], nl = {"\n", STRLEN("\n")};
	struct utsname u;
	char *names[] = {u.sysname, u.nodename, u.release, u.version, u.machine};
	size_t c = 0, i;

	ARGBEG("asnrvm") {
	case 'a':
		flags[sFLAG] = flags[nFLAG] = flags[rFLAG] = flags[vFLAG] = flags[mFLAG] = 1;
		break;
	case 's':
		flags[sFLAG] = 1;
		break;
	case 'n':
		flags[nFLAG] = 1;
		break;
	case 'r':
		flags[rFLAG] = 1;
		break;
	case 'v':
		flags[vFLAG] = 1;
		break;
	case 'm':
		flags[mFLAG] = 1;
		break;
	default:
		usage();
	} ARGEND

	if (argc - optind)
		usage();

	if (!(flags[nFLAG] || flags[rFLAG] || flags[vFLAG] || flags[mFLAG]))
		flags[sFLAG] = 1;

	if (uname(&u) == -1)
		func_error_exit("uname");

	for (i = 0; i < FLAGSIZE; ++i)
		if (flags[i]) {
			struct iovec tmp = {
				names[i], strlen(names[i])
			}, sp = {
				" ", STRLEN(" ")
			};
			iov[c++] = tmp;
			iov[c++] = sp;
		}

	iov[c++] = nl;
	if (writevn(1, iov, c) == -1)
		func_error_exit("writev");

	_exit(0);
}
