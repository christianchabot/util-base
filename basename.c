#include <libgen.h>
#include <string.h>
#include <unistd.h>

#include "io.h"

#define FLAGSSTR " string [suffix]\n"

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
	void **path = &iov->iov_base;

	ARGBEG("") {
	default:
		usage();
	} ARGEND

	if (argc-optind > 2)
		usage();

	*path = basename(*argv);
	if (*(argv+1)) {
		size_t off = strlen(*path) - strlen(*(argv+1));
		if (!strcmp((char *) *path+off, *(argv+1)))
			*((char *) *path+off) = '\0';
	}

	iov->iov_len = strlen(*path);
	if (writevn(1, iov, ARRSIZ(iov)) == -1)
		func_error_exit("writev");

	_exit(0);
}
