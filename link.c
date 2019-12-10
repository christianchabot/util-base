#include <string.h>
#include <unistd.h>

#include "io.h"

#define FLAGSSTR " file1 file2\n"

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

	if (argc-optind != 2)
		usage();

	if (link(*argv, *(argv+1)))
		file_error_exit(*argv);

	_exit(0);
}
