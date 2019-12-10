#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "io.h"

#define FLAGSSTR " [-i] [name=value]... [utility [argument...]]\n"

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
	extern char **environ;

	ARGBEG("i") {
	case 'i':
		*environ = NULL;
		break;
	default:
		usage();
	} ARGEND

	while (*argv && strchr(*argv, '='))
		if (putenv(*argv++))
			func_error_exit("putenv");

	if (*argv) {
		int serrno = (execvp(*argv, argv), errno);
		func_error("execvp");
		_exit(126 + (serrno == ENOENT));
	}

	while (*environ)
		write_strings(NULL, *environ++, "\n", NULL);
	write_strings(ioflush, NULL);
	_exit(0);
}
