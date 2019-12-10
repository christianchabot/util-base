#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "io.h"

#define FLAGSSTR " utility [argument...]\n"

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
	argv0 = *argv, argv0 ? (--argc, ++argv) : (void *) 0;
	signal(SIGHUP, SIG_IGN);
	if (isatty(1)) {
		char buf[PATH_MAX + STRLEN("nohup.out")] = "nohup.out", *home;

		if (close(1) == -1)
			func_error_exit("close");

		if (open("nohup.out", O_APPEND | O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR) == -1) {
			if (!(home = getenv("HOME")))
				name_errors_exit("HOME cannot be found in the environment\n", NULL);

			if (!(home = memccpy(buf, home, '\0', sizeof(buf))))
				name_errors_exit("HOME in the environment exceeds PATH_MAX\n", NULL);

			strcpy(home - 1, "nohup.out");
			if (open(buf, O_APPEND | O_WRONLY | O_CREAT, (S_IRUSR | S_IWUSR) & umask(0)) == -1)
				func_file_error_exit("open", buf);
		}

		name_errors("appending to: ", buf, "\n", NULL);
	}

	if (isatty(2))
		close(2) == -1 ? func_error_exit("close") : dup(1);

	if (isatty(0)) {
		if (close(0) == -1)
			func_error_exit("close");

		if (open("/dev/null", O_WRONLY) == -1)
			func_file_error_exit("open", "/dev/null");
	}

	execvp(*argv, argv);
	_exit(126 + (errno == ENOENT));
}
