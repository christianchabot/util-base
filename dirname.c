#include <libgen.h>
#include <string.h>
#include <unistd.h>

#include "io.h"

#define FLAGSSTR " string\n"

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
	struct iovec iov[] = {
		NULL, 0,
		"\n", STRLEN("\n")
	};

	ARGBEG("") {
	default:
		usage();
	} ARGEND

	if (argc-optind != 1)
		usage();

	iov->iov_base = dirname(*argv);
	iov->iov_len = strlen(iov->iov_base);
	if (writevn(1, iov, ARRSIZ(iov)) == -1)
		func_error_exit("writev");

	_exit(0);
}
