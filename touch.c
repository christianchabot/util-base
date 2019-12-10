#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <time.h>
#include <unistd.h>

#include "io.h"

#define FLAGSSTR " [-acm] [-r ref_file | -t time | -d date_time] file...\n"

enum {aFLAG, cFLAG, dFLAG, mFLAG, rFLAG, tFLAG, FLAGSIZE};
static unsigned flags[FLAGSIZE];
static struct timespec times[2];

static int
touch(const char *file)
{
	if (utimensat(AT_FDCWD, file, times, 0) == -1) {
		int fd, ret;

		if (errno != ENOENT) {
			func_file_error("utimensat", file);
			return 1;
		}

		if (flags[cFLAG])
			return 0;

		fd = creat(file, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
		if (fd == -1) {
			func_file_error("creat", file);
			return 1;
		}

		ret = futimens(fd, times);
		close(fd);
		if (ret == -1) {
			func_file_error("futimens", file);
			return 1;
		}
	}

	return 0;
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

static int
parsetime(const char *time)
{
	const char *fmt;
	struct tm tm = {0};

	switch (strlen(time)) {
	case 8:
		fmt = "%m%d%H%M";
		break;
	case 9:
		fmt = "%m%d%H%M.%S";
		break;
	case 10:
		fmt = "%y%m%d%H%M";
		break;
	case 12:
		fmt = "%Y%m%d%H%M";
		break;
	case 13:
		fmt = "%y%m%d%H%M.%S";
		break;
	case 15:
		fmt = "%Y%m%d%H%M.%S";
		break;
	default:
		name_errors_exit("invalid date format: ", time, "\n", NULL);
	}

	if (!strptime(time, fmt, &tm))
		name_errors_exit("invalid date format: ", time, "\n", NULL);

	times->tv_sec = mktime(&tm);
	if (times->tv_sec == -1)
		name_errors_exit("invalid date format: ", time, "\n", NULL);
}

static void
parsedate(const char *date)
{
	const char *tmp;
	long fracsec = 0;
	struct tm tm = {0};

	tm.tm_isdst = -1;
	times->tv_nsec = 0;
	tmp = strptime(date, "%Y-%m-%d", &tm);
	if (!tmp || !(*tmp == ' ' || *tmp == 'T'))
		name_errors_exit("invalid date format: ", date, "\n", NULL);

	tmp = strptime(++tmp, "%H:%M:%S", &tm);
	if (!tmp)
		name_errors_exit("invalid date format: ", date, "\n", NULL);

	if (*tmp == '.' || *tmp == ',') {
		char *end = strptime(++tmp, "%e", &tm), save;
		if (!end)
			name_errors_exit("invalid date format: ", date, "\n", NULL);

		times->tv_nsec = strtol(tmp, NULL, 10);
		if (!times->tv_nsec && errno)
			name_errors_exit("invalid date format: ", date, "\n", NULL);

		/* ***** figure out the fraction of seconds part ***** */
		times->tv_nsec *= 1000000000; /* i dont think this is right */
		tmp = end;
	}

	if (!*tmp)
		unsetenv("TZ");
	else
		if (*tmp != 'Z')
			name_errors_exit("invalid date format: ", date, "\n", NULL);

	times->tv_sec = mktime(&tm);
	if (times->tv_sec == -1)
		name_errors_exit("invalid date format: ", date, "\n", NULL);
}

int
main(int argc, char *argv[])
{
	int ret = 0;
	struct stat ref;

	ARGBEG("acd:mr:t:") {
	case 'a':
		flags[aFLAG] = 1;
		break;
	case 'c':
		flags[cFLAG] = 1;
		break;
	case 'd':
		flags[dFLAG] = 1;
		parsedate(optarg);
		break;
	case 'm':
		flags[mFLAG] = 1;
		break;
	case 'r':
		flags[rFLAG] = 1;
		if (stat(optarg, &ref) == -1)
			func_file_error_exit("stat", optarg);
		times->tv_nsec = ref.st_atime;
		(times + 1)->tv_nsec = ref.st_mtime;
		break;
	case 't':
		flags[tFLAG] = 1;
		parsetime(optarg);
		break;
	default:
		usage();
	} ARGEND

	if (argc-optind < 1)
		usage();

	if (flags[dFLAG] + flags[rFLAG] + flags[tFLAG] > 1)
		usage();

	if (!flags[aFLAG] && !flags[mFLAG])
		flags[aFLAG] = flags[mFLAG] = 1;

	if (!flags[rFLAG])
		*(times + 1) = *times;
	if (!flags[aFLAG])
		times->tv_nsec = UTIME_OMIT;
	if (!flags[mFLAG])
		(times + 1)->tv_nsec = UTIME_OMIT;

	do
		if (touch(*argv))
			ret = 1;
	while (*++argv);

	_exit(ret);
}
