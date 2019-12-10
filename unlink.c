#include <string.h>
#include <unistd.h>

#include "io.h"

#define FLAGSSTR " file\n"

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
	ARGBEG("") {
	default:
		usage();
	} ARGEND

	if (argc-optind != 1)
		usage();

	if (unlink(*argv))
		file_error_exit(*argv);

	_exit(0);
}
