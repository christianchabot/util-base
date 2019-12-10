#include <string.h>
#include <sys/uio.h>
#include <unistd.h>

#include "io.h"

static void
usage(void)
{
	USAGEBEG {
		USAGESTR, STRLEN(USAGESTR),
		argv0, strlen(argv0),
		"\n", STRLEN("\n")
	} USAGEEND
}

int
main(int argc, char *argv[])
{
	struct iovec iov[] = {
		NULL, 0,
		"\n", STRLEN("\n")
	};

	argv0 = *argv, argv0 ? (--argc, ++argv) : (void *) 0;
	if (*argv)
		usage();

	if (!(iov->iov_base = getlogin()))
		name_errors_exit("no login name\n", NULL);

	iov->iov_len = strlen(iov->iov_base);
	if (writevn(1, iov, ARRSIZ(iov)) == -1)
		func_error_exit("writev");

	_exit(0);
}
