#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/uio.h>
#include <time.h>

#include "io.h"

#define FLAGSSTR " [[month] year]\n"

static void
cal(int month, int year)
{
	/* print the calender */
}

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
	long month = 0, year = 0;

	argv0 = *argv, argv0 ? (--argc, ++argv) : (void *) 0;

	if (argc > 2)
		usage();

	switch (argc) {
	case 2:
		month = strtol(*argv, NULL, 10);
		if (errno || month < 1 || month > 12)
			name_errors_exit("invalid month: ", *argv, "\n", NULL);
		++argv;
	case 1:
		year = strtol(*argv, NULL, 10);
		if (errno || year < 1 || year > 9999)
			name_errors_exit("invalid year: ", *argv, "\n", NULL);
	case 0:
		break;
	default:
		usage();
	}

	cal(month, year);
	_exit(0);
}
