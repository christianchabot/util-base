#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "io.h"

#define FLAGSSTR " time\n"

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
	unsigned seconds = 0;

	ARGBEG("") {
	default:
		usage();
	} ARGEND

	if (argc - optind != 1)
		usage();

	seconds = strtoul(*argv, NULL, 10) & ~0u;
	if (seconds)
		while (seconds = sleep(seconds));

	_exit(0);
}
