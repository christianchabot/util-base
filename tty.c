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
	char *tty;
	struct iovec iov[] = {
		"not a tty\n", STRLEN("not a tty\n"),
		"\n", STRLEN("\n")
	};
	ssize_t r;

	ARGBEG("") {
	default:
		usage();
	} ARGEND

	if (argc-optind)
		usage();

	if (tty = ttyname(0))
		iov->iov_base = tty, iov->iov_len = strlen(iov->iov_base);

	r = writevn(1, iov, ARRSIZ(iov));
	if (r == -1)
		func_error("writev");

	_exit(r == -1 ? 2 : tty ? 0 : 1);
}
