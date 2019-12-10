#include <string.h>
#include <sys/uio.h>
#include <utmpx.h>

#include "io.h"

#define FLAGSSTR " [[-mTuabdHlprt] [file] | [-musbHlprt] [file] | -q [file] | am i | am I]\n"

#define ZEROALL (flags[bFLAG] = flags[dFLAG] = flags[HFLAG] = flags[lFLAG] = flags[mFLAG] = flags[pFLAG] = flags[rFLAG] = flags[sFLAG] = flags[tFLAG] = flags[uFLAG] = 0)
#define SETAFLAG (flags[bFLAG] = flags[dFLAG] = flags[lFLAG] = flags[pFLAG] = flags[rFLAG] = flags[tFLAG] = flags[uFLAG] = 1)

enum {bFLAG, dFLAG, HFLAG, lFLAG, mFLAG, pFLAG, qFLAG, rFLAG, sFLAG, tFLAG, TFLAG, uFLAG, FLAGSIZE};
static unsigned flags[FLAGSIZE];

static struct utmpx utmp[FLAGSIZE]; /* leave for now */

static struct {
	const char *header;
	size_t size;
} fields[] = {
	"NAME", strlen("NAME"),
	"STATE", strlen("STATE"),
	"LINE", strlen("LINE"),
	"TIME", strlen("TIME"),
	"ACTIVITY", strlen("ACTIVITY"),
	"PID", strlen("PID"),
	"COMMENT", strlen("COMMENT"),
	"EXIT", strlen("EXIT")
};

static void
print_utmp(void)
{
	size_t qcount = 0;
	struct utmpx *utmp;

	/* todo:
		bFLAG,
		
	*/

	if (flags[HFLAG]) {
		/* flags that print NAME */
		/* flags that print STATE */
		/* flags that print LINE */
		/* flags that print TIME */
		/* flags that print ACTIVITY */
		/* flags that print PID */
		/* flags that print COMMENT */
		/* flags that print EXIT */
	}

	setutxent();
	while (utmp = getutxent()) {
		switch (utmp->ut_type) {
		case EMPTY:
			break;
		case BOOT_TIME:
			if (flags[bFLAG]) {
				/* print bFLAG */
			}
			break;
		case OLD_TIME:
			break;
		case NEW_TIME:
			break;
		case USER_PROCESS:
			break;
		case INIT_PROCESS:
			break;
		case LOGIN_PROCESS:
			break;
		case DEAD_PROCESS:
			break;
		}
	}
	endutxent();
}

static void
usage(void)
{
	USAGEBEG {
		USAGESTR, STRLEN(USAGESTR),
		argv0, strlen(argv0),
		FLAGSSTR STRLEN(FLAGSSTR)
	} USAGEEND
}

int
main(int argc, char *argv[])
{
	/* check for 'am i' or 'am I' here */

	ARGBEG("abdHlmpqrstTu") {
	case 'a':
		SETAFLAG; break;
	case 'b':
		flags[bFLAG] = 1; break;
	case 'd':
		flags[dFLAG] = 1; break;
	case 'H':
		flags[HFLAG] = 1; break;
	case 'l':
		flags[lFLAG] = 1; break;
	case 'm':
		flags[mFLAG] = 1; break;
	case 'p':
		flags[pFLAG] = 1; break;
	case 'q':
		ZEROALL; flags[qFLAG] = 1; break;
	case 'r':
		flags[rFLAG] = 1; break;
	case 's':
		flags[sFLAG] = 1; break;
	case 't':
		flags[tFLAG] = 1; break;
	case 'T':
		flags[TFLAG] = 1; break;
	case 'u':
		flags[uFLAG] = 1; break;
	default:
		usage();
	} ARGEND

	print_utmp();
	_exit(0);
}
