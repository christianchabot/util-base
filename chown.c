#include <ftw.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "ch.h"
#include "io.h"
#include "util.h"

#define FLAGSSTR1 " [-h] owner[:group] file...\n"
#define FLAGSSTR2 " -R [-H | -L | -P] owner[:group] file...\n"

enum {HFLAG, LFLAG, hFLAG, PFLAG, RFLAG, FLAGSIZE};
static unsigned flags[FLAGSIZE];
static int ret;

static int
wchown(const char *path, const struct stat *st, int info, struct FTW *ftw)
{
	if (call.rchfn(path + ftw->base, call.uid, call.gid) == -1)
		func_file_error(call.rchname, path);

	return 0;
}

static void
usage(void)
{
	USAGEBEG {
		USAGESTR, STRLEN(USAGESTR),
		argv0, strlen(argv0),
		FLAGSSTR1, STRLEN(FLAGSSTR1),
		USAGEALN, STRLEN(USAGEALN),
		argv0, strlen(argv0),
		FLAGSSTR2, STRLEN(FLAGSSTR2)
	} USAGEEND
}

int
main(int argc, char *argv[])
{
	struct passwd *pw;
	char *group;

	ARGBEG("hHLPR") {
	case 'h':
		flags[hFLAG] = 1;
		call = funcs[hFLAG];
		break;
	case 'H':
		flags[HFLAG] = 1;
		call = funcs[HFLAG];
		break;
	case 'L':
		flags[LFLAG] = 1;
		call = funcs[LFLAG];
		break;
	case 'P':
		flags[PFLAG] = 1;
		call = funcs[PFLAG];
		break;
	case 'R':
		flags[RFLAG] = 1;
		call = funcs[LFLAG];
		break;
	default:
		usage();
	} ARGEND

	if (argc-optind < 2 || flags[hFLAG] + flags[RFLAG] > 1)
		usage();
	else if (!flags[RFLAG] && (flags[HFLAG] || flags[PFLAG] || flags[LFLAG]))
		usage();

	if (group = strchr(*argv, ':'))
		*group++ = '\0', call.gid = xstrtogid(group);

	call.uid = xstrtouid(*argv);
	while (*++argv) {
		if (call.chfn(*argv, call.uid, call.gid) == -1) {
			func_file_error(call.chname, *argv);
			ret = 1;
			continue;
		}

		if (flags[RFLAG]) {
			struct stat st;

			if (call.stfn(*argv, &st) == -1) {
				func_file_error(call.stname, *argv);
				ret = 1;
				continue;
			}

			if (S_ISDIR(st.st_mode) && nftw(*argv, wchown, 17, call.flags) == -1) {
				func_file_error("nftw", *argv);
				ret = 1;
			}
		}
	}

	_exit(ret);
}
