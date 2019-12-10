#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/uio.h>
#include <time.h>

#include <locale.h>

#include "io.h"

#define FLAGSSTR " [-u] [+format | mmddHHMM[[CC]yy]]\n"

static void
usage(void)
{
	IOVBEG {
		USAGESTR, STRLEN(USAGESTR),
		argv0, strlen(argv0),
		FLAGSSTR, STRLEN(FLAGSSTR)
	} IOVEND
}

static void
setdate(const char *str, struct tm *tm)
{
	struct tm set = *tm;
	const char *base = str;
	int *dateptr[] = {
		&set.tm_mon,
		&set.tm_mday,
		&set.tm_hour,
		&set.tm_min,
		&set.tm_year
	}, **p = dateptr;
	struct timespec ts = {0};

	do {
		if (!isdigit(*str) || !isdigit(*(str + 1)))
			goto error;
		**p++ = (*str - '0')*10 + *(str + 1) - '0';
	} while ((str = str + 2) < base+8);

	if (*str != '\0') {
		if (!isdigit(*str) || !isdigit(*(str + 1)))
			goto error;
		**p = (*str - '0')*10 + *(str + 1) - '0';
		if (*(str + 2) != '\0') {
			if (!isdigit(*(str + 2)) || !isdigit(*(str + 3)))
				goto error;
			
			if (*(str + 4) != '\0')
				goto error;
			**p = (**p - 19)*100 + (*(str + 2) - '0')*10 + *(str + 3) - '0';
		} else
			if (set.tm_year < 69)
				set.tm_year += 100;
	}

	set.tm_mon -= 1;
	set.tm_isdst = -1;
	ts.tv_sec = mktime(&set);
	if (ts.tv_sec == -1)
		goto error;

	if (clock_settime(CLOCK_REALTIME, &ts) == -1)
		func_error_exit("clock_settime");

	return;
error:
	name_errors_exit("invalid date format: ", base, "\n", NULL);
}

int
main(int argc, char *argv[])
{
	char buf[BUFSIZ], *fmt = "%a %b %e %H:%M:%S %Z %Y";
	struct iovec iov[] = {
		buf, 0,
		"\n", STRLEN("\n")
	};
	struct tm *tm;
	time_t t = time(NULL);

	if (t == -1)
		func_error_exit("time");

	ARGBEG("u") {
	case 'u':
		if (putenv("TZ=UTC0"))
			func_error_exit("putenv");
		break;
	default:
		usage();
	} ARGEND

	if (argc-optind > 1)
		usage();

	setlocale(LC_ALL, "");

	tm = localtime(&t);
	if (!tm)
		func_error_exit("localtime");

	if (*argv) {
		if (**argv == '+')
			fmt = *argv + 1;
		else {
			setdate(*argv, tm);
			_exit(0);
		}
	}

	if (strftime(buf, sizeof(buf), fmt, tm))
		iov->iov_len = strlen(iov->iov_base);

	if (writevn(1, iov, ARRSIZ(iov)) == -1)
		func_error_exit("writev");

	_exit(0);
}
