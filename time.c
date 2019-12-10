#include <errno.h>
#include <float.h>
#include <stdio.h>
#include <string.h>
#include <sys/times.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include "io.h"

#define FLAGSSTR " [-p] utility [argument...]\n"

/* Fix all of this. */
#define TOSTR(str) #str
#define STRVAL(str) TOSTR(str)

#define DBLDIGITS (3 + DBL_MANT_DIG - DBL_MIN_EXP) /* Includes decimal and sign */

#define FMTSTR "real %f\nuser %f\nsys %f\n"
#define FMTSTRSIZ (STRLEN(FMTSTR)-STRLEN("%f")*3)

#define DEFAULT_PRECISION 6
#define FMTBUFSIZ (1 + FMTSTRSIZ + DEFAULT_PRECISION*3 + DBLDIGITS*3) /* includes \0 */


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
	size_t len;
	long ticksiz;
	int ret = 0, stat_loc;
	struct tms tms;
	pid_t pid;
	clock_t rstart, rend;
	char buf[FMTBUFSIZ];

	ARGBEG("p") {
	case 'p':
		break;
	default:
		usage();
	} ARGEND

	if (argc-optind < 1)
		usage();

	if ((ticksiz = sysconf(_SC_CLK_TCK)) == -1)
		func_error_exit("sysconf");

	rstart = times(&tms);
	if (rstart == -1)
		_exit(1);

	pid = fork();
	if (pid == -1)
	 	func_error_exit("fork");
	else if (pid == 0) {
		execvp(*argv, argv);
		_exit(126 + (errno == ENOENT));
	}
	waitpid(pid, &stat_loc, 0);

	rend = times(&tms);
	if (rend == -1)
		_exit(1);

	/* Handle job control signals? */
	if (WIFEXITED(stat_loc))
		ret = WEXITSTATUS(stat_loc);
	else if (WIFSIGNALED(stat_loc))
		ret = WIFSIGNALED(stat_loc);

	len = sprintf(buf, "real %f\nuser %f\nsys %f\n", 
		(rend-rstart) / (double)ticksiz,
		(tms.tms_utime+tms.tms_cutime) / (double)ticksiz,
		(tms.tms_stime+tms.tms_cstime) / (double)ticksiz);

	if (writen(2, buf, len) == -1)
		func_error_exit("write");

	_exit(ret);
}
