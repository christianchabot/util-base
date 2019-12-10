#include <sys/stat.h>
#include <sys/statvfs.h>
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
	struct statvfs vfs;
	int fd;

	argv0 = *argv, argv0 ? (--argc, ++argv) : (void *) 0;

	if (argc != 1)
		usage();

	fd = statvfs(*argv, &vfs);
	

	_exit(0);
}
