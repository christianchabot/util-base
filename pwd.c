#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <unistd.h>

#include "io.h"

#define FLAGSSTR " [-L | -P]\n"

static unsigned Lflag = 1;

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
	char cwd[PATH_MAX], rescwd[PATH_MAX];
	struct iovec iov[] = {
		cwd, ARRSIZ(cwd),
		"\n", STRLEN("\n")
	};

	ARGBEG("LP") {
	case 'L':
		Lflag = 1;
		break;
	case 'P':
		Lflag = 0;
		break;
	default:
		usage();
	} ARGEND

	if (argc-optind > 0)
		usage();

	iov->iov_base = getcwd(cwd, sizeof(cwd));
	if (!iov->iov_base)
		func_error_exit("getcwd");

	if (Lflag) {
		char *tmp = getenv("PWD");
		if (tmp && tmp[0] == '/') {
			struct stat pst, cst;
			if (stat(cwd, &cst) != -1 && stat(tmp, &pst) != -1)
				if ((pst.st_dev == cst.st_dev) && (pst.st_ino == cst.st_ino))
					if (tmp = realpath(tmp, rescwd))
						iov->iov_base = tmp;
		}
	}

	iov->iov_len = strlen(iov->iov_base);
	if (writevn(1, iov, ARRSIZ(iov)) == -1)
		func_error_exit("writev");

	_exit(0);
}
