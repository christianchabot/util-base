#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/uio.h>

#include "io.h"

#define FLAGSSTR " [-n number] [file...]\n"

static long nflag = 10;

static void
usage(void)
{
	USAGEBEG {
		USAGESTR, STRLEN(USAGESTR),
		argv0, strlen(argv0),
		FLAGSSTR, STRLEN(FLAGSSTR)
	} USAGEEND;
}

int
main(int argc, char *argv[])
{
	struct iovec iov[16];
	ssize_t r, t;
	unsigned lines;
	int curiov = 0, fd, ret = 0, header = 0;
	char buf[BUFSIZ], *ptr;

	ARGBEG("n:") {
	case 'n':
		nflag = strtol(optarg, NULL, 10);
		if (errno || nflag < 0)
			name_errors_exit("invalid option argument: ", optarg, "\n", NULL);
		break;
	default:
		usage();
	} ARGEND

	header = argc-optind > 1;
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

		if (header) {
			struct iovec tmp[] = {
				"\n", STRLEN("\n"),
				"==> ", STRLEN("==> "),
				*argv, strlen(*argv),
				" <==\n", STRLEN(" <==\n"),
			}, *ptr = tmp, *base = tmp + ARRSIZ(tmp);

			if (header < 2) {
				ptr = tmp+1;
				++header;
			}

			do {

				iov[curiov++] = *ptr;
				if (curiov >= ARRSIZ(iov)) {
					if (writevn(1, iov, ARRSIZ(iov)) == -1)
						func_error_exit("write");
					curiov = 0;
				}
			} while (++ptr < base);
		}

		if (!fd) {
			if (writevn(1, iov, curiov) == -1)
				func_error_exit("write");
			curiov = 0;
		}

		lines = nflag;
		while (lines && (r = read(fd, buf, ARRSIZ(buf)))) {
			struct iovec tmp = {buf, 0};

			if (r == -1) {
				file_error(*argv);
				break;
			}

			for (t = 0; t < r && lines && (ptr = memchr(buf+t, '\n', r-t)); --lines, t = ptr-buf+1);

			tmp.iov_len = ptr ? t : r;
			iov[curiov++] = tmp;
			if (writevn(1, iov, curiov) == -1)
				func_error_exit("write");
			curiov = 0;
		}

		if (fd)
			close(fd);
	} while (*argv && *++argv);

	if (curiov)
		if (writevn(1, iov, curiov) == -1)
			func_error_exit("write");

	_exit(0);
}
