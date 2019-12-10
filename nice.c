#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/uio.h>
#include <unistd.h>

#include "io.h"

#define FLAGSSTR " [-n increment] utility [argument...]\n"

static long nflag;

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
	int olderrno;

	ARGBEG("n:") {
	case 'n':
		nflag = strtol(optarg, NULL, 10);
		if (errno)
			name_errors_exit("invalid nice value: ", optarg, "\n", NULL);
		break;
	default:
		usage();
	} ARGEND

	if (argc-optind < 1)
		usage();

	if (nflag && nice(nflag) == -1)
		func_error_exit("nice");

	olderrno = errno;
	execvp(*argv, argv);
	func_error("execvp");
	_exit(126 + (olderrno == ENOENT));

}
