#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <sys/uio.h>

#include "io.h"

#define FLAGSSTR " [-l | -s] file1 file2\n"

#define TOSTR(x) #x
#define VALSTR(x) TOSTR(x)

#define OCTALSIZ 3
#define FMTSIZ (STRLEN(VALSTR(INT_MAX)) + STRLEN(" ")*2 + OCTALSIZ*2 + STRLEN("\n") + 1)

static unsigned flag;

static void
usage(void)
{
	USAGEBEG {
		USAGESTR, STRLEN(USAGESTR),
		argv0, strlen(argv0),
		FLAGSSTR, STRLEN(FLAGSSTR)
	} USAGEEND
}

static void
write_stats(char *names[2], size_t byte, size_t line)
{
	
	char bufc[STRLEN(VALSTR(INT_MAX))+1], bufl[STRLEN(VALSTR(INT_MAX))+1];
	struct iovec tmp[] = {
		names[0], strlen(names[0]),
		" ", STRLEN(" "),
		names[1], strlen(names[1]),
		" differ: char ", STRLEN(" differ: char "),
		bufc, sprintf(bufc, "%d", byte),
		" line ", STRLEN(" line "),
		bufl, sprintf(bufl, "%d", line),
		"\n", STRLEN("\n")
	};

	if (writevn(1, tmp, ARRSIZ(tmp)) == -1)
		func_error_exit("writev");
}

static int
cmp(int fds[2], char *names[2])
{
	size_t byte = 1, line = 1, i1 = 0, i2 = 0, io = 0;
	ssize_t r1 = 0, r2 = 0;
	int ret = 0;
	char buf1[BUFSIZ/2], buf2[BUFSIZ/2], bufout[BUFSIZ];

	do {
		if (i1 >= r1) {
			r1 = read(fds[0], buf1, sizeof(buf1));
			if (r1 == -1)
				file_error_exit(names[0]);
			i1 = 0;
		}

		if (i2 >= r2) {
			r2 = read(fds[1], buf2, sizeof(buf2));
			if (r2 == -1)
				file_error_exit(names[1]);
			i2 = 0;
		}

		for (; i1 < r1 && i2 < r2; ++i1, ++i2, ++byte)
			if (buf1[i1] != buf2[i2])
				if (flag != 'l') {
					if (!flag)
						write_stats(names, byte, line);

					return 1;
				} else {
					if (io + FMTSIZ >= sizeof(bufout)) {
						if (writen(1, bufout, io) == -1)
							func_error_exit("write");
						io = 0;
					}
					io += sprintf(bufout+io, "%d %o %o\n", byte, buf1[i1], buf2[i2]);
					ret = 1;
				}
			else
				if (buf1[i1] == '\n')
					++line;
	} while (r1 && r2);

	if (flag != 's' && (r1 || r2) && !(r1 && r2)) {
		struct iovec tmp[] = {
			argv0, strlen(argv0),
			": EOF on ", STRLEN(": EOF on "),
			!r1 ? names[0] : names[1], strlen(!r1 ? names[0] : names[1]),
			"\n", STRLEN("\n")
		};

		if (writevn(2, tmp, ARRSIZ(tmp)) == -1)
			func_error_exit("writev");
	}

	if (io)
		if (writen(1, bufout, io) == -1)
			func_error_exit("write");

	return ret;
}

#undef VALSTR
#undef TOSTR

int
main(int argc, char *argv[])
{
	char **nptr;
	int fds[2], i, ret;

	ARGBEG("ls") {
	case 'l':
		flag = 'l';
		break;
	case 's':
		flag = 's';
		break;
	default:
		usage();
	} ARGEND

	if (argc-optind != 2)
		usage();

	for (i = 0; i < ARRSIZ(fds); ++i)
		if (*argv[i] != '-' || *(argv[i]+1) != '\0') {
			fds[i] = open(argv[i], O_RDONLY);
			if (fds[i] == -1)
				file_error_exit(argv[i]);
		} else
			fds[i] = 0;

	ret = cmp(fds, argv);
	for (i = 0; i < ARRSIZ(fds); ++i)
		if (fds[i])
			if (close(fds[i]) == -1)
				name_error_exit();

	_exit(ret);
}
