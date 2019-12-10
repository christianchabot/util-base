#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#include "io.h"

#define FLAGSSTR " [-u] [file...]\n"

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
	ssize_t (*rdwrptr) (int) = rdfd_wrout_buf;
	int fd, ret = 0;

	ARGBEG("u") {
	case 'u':
		rdwrptr = rdfd_wrout;
		break;
	default:
		usage();
	} ARGEND

	do {
		if (*argv && (**argv != '-' || *(*argv+1) != '\0')) {
			fd = open(*argv, O_RDONLY);
			if (fd == -1) {
				file_error(*argv);
				ret = 1;
				continue;
			}
		} else
			fd = 0;

		if (rdwrptr(fd) == -1) {
			if (fd)
				file_error(*argv);
			ret = 1;
		}

		if (fd != 0)
			close(fd);
	} while (*argv && *++argv);
	rdwrout_flush();
	_exit(ret);
}
