#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/uio.h>
#include <unistd.h>

#include "io.h"

#define USAGEFLAG1 " [-s signal_name | -signal_name | -signal_number] pid...\n"
#define USAGEFLAG2 " -l [exit_status]\n"


struct sigt {
	char *name, *val;
	const int sig;
} sigs[] = {
	{"0", "0", 0},
#define TOSTR(x) #x
#define VALSTR(x) TOSTR(x)
#define SIG(x) { #x, VALSTR(SIG##x), SIG##x }
	SIG(ABRT), SIG(ALRM), SIG(BUS), SIG(CHLD), SIG(CONT), SIG(FPE), SIG(HUP),
	SIG(ILL), SIG(INT), SIG(KILL), SIG(PIPE), SIG(QUIT), SIG(SEGV), SIG(STOP),
	SIG(TERM), SIG(TSTP), SIG(TTIN), SIG(TTOU), SIG(USR1), SIG(USR2), SIG(POLL),
	SIG(PROF), SIG(SYS), SIG(TRAP), SIG(URG), SIG(VTALRM), SIG(XCPU), SIG(XFSZ)
#undef SIG
#undef VALSTR
#undef TOSTR
};

enum {lFLAG, longsFLAG, sFLAG, FLAGSIZE};
static char *sig_name, *lopt;
static unsigned flags[FLAGSIZE];
static struct sigt *sigsend = &sigs[15]; /* initialized to SIGTERM */

/* might change later */
static int
sig_match(const char *name, const char *val)
{
	const char *cmp = val ? val : name ? name : NULL;
	size_t i, valoff = (val ? (char *) &sigs[0].val : (char *) &sigs[0].name) - (char *) &sigs[0];

	if (cmp)
		for (i = 0; i < ARRSIZ(sigs); ++i) {
			char *b = (char *) &sigs[i]+valoff;
			if (!strcasecmp(cmp, *(char **) b))
				return i;
		}

	return -1;
}

static void
write_sig(const char *val)
{
	struct iovec iov[] = {
		NULL, 0,
		"\n", STRLEN("\n")
	};
	size_t i;

	if ((i = sig_match(NULL, val)) == -1) {
		name_errors("invalid signal: ", val, "\n", NULL);
		return;
	}

	iov->iov_base = sigs[i].name;
	iov->iov_len = strlen(iov->iov_base);
	if (writevn(1, iov, ARRSIZ(iov)) == -1)
		func_error_exit("write");
}

static void
dump_sigs(void)
{
	struct iovec iov[16];
	size_t i, j;

	for (i = 0; i < ARRSIZ(sigs);) {
		for (j = 0; j < ARRSIZ(iov) && i < ARRSIZ(sigs); j += 2, ++i) {
			iov[j].iov_base = sigs[i].name;
			iov[j].iov_len = strlen(sigs[i].name);
			iov[j+1].iov_base = "\n";
			iov[j+1].iov_len = STRLEN("\n");
		}
		if (writevn(1, iov, j) == -1)
			func_error_exit("write");
	}
}

static int 
long_opts(char *argv)
{
	size_t i;
	int ret = -1;

	for (i = 0; i < ARRSIZ(sigs); ++i)
		if (!strcasecmp(argv, sigs[i].name) || !strcasecmp(argv, sigs[i].val)) {
			sigsend = &sigs[i];
			ret = 0;
		}

	return ret;
}

static void
usage(void)
{
	USAGEBEG {
		USAGESTR, STRLEN(USAGESTR),
		argv0, strlen(argv0),
		USAGEFLAG1, STRLEN(USAGEFLAG1),
		USAGEALN, STRLEN(USAGEALN),
		argv0, strlen(argv0),
		USAGEFLAG2, STRLEN(USAGEFLAG2),
	} USAGEEND
}

int
main(int argc, char *argv[])
{
	char *tmp;
	int c, r, ret = 0;

	sig_name = "TERM";
	ARGBEG(":l:s:") {
		case 's':
			if ((r = sig_match(optarg, NULL)) == -1)
				name_errors_exit("invalid signal: ", optarg, "\n", NULL);
			sigsend = &sigs[r];
			flags[sFLAG] = 1;
			break;
		case 'l':
			flags[lFLAG] = 1;
			lopt = optarg;
			break;
		case ':':
			if (optopt == 'l')
				flags[lFLAG] = 1;
			break;
		default:
			usage();
	} ARGEND

	/* Parse longopts. */
	if (*argv && **argv == '-') {
		r = long_opts(*argv+1);
		if (r != -1)
			++argv, flags[longsFLAG] = 1;
	}

	if (flags[lFLAG] + flags[longsFLAG] + flags[sFLAG] > 1)
		usage();

	if (flags[lFLAG]) {
		if (!lopt)
			dump_sigs();
		else
			write_sig(lopt);
	}

	/* send signal to pid(s) */
	while (tmp = *argv++) {
		pid_t pid = strtol(tmp, NULL, 10);
		if (pid == -1) {
			name_errors("invalid signal: ", tmp, "\n", NULL);
			ret = 1;
			continue;
		}

		if (kill(pid, sigsend->sig) == -1) {
			func_file_error("kill", tmp);
			ret = 1;
		}
	}

	_exit(ret);
}
