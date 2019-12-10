#include <ftw.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "ch.h"
#include "io.h"
#include "util.h"

#define FLAGSSTR " [-h | -R [-H | -L | -P]] group file...\n"

enum {HFLAG, LFLAG, hFLAG, PFLAG, RFLAG, FLAGSIZE};
static unsigned flags[FLAGSIZE];
static int ret;

static int
wchgrp(const char *path, const struct stat *st, int info, struct FTW *ftw)
{
	if (call.rchfn(path + ftw->base, -1, call.gid) == -1) {
		func_file_error(call.rchname, path + ftw->base);
		ret = 1;
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

int
main(int argc, char *argv[])
{
	struct group *grp;

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

	if (argc-optind < 2 || flags[hFLAG] + flags[RFLAG] > 2)
		usage();
	else if (!flags[RFLAG] && (flags[HFLAG] || flags[LFLAG] || flags[PFLAG]))
		usage();

	call.gid = xstrtogid(*argv);
	while (*++argv) {
		if (call.chfn(*argv, -1, call.gid) == -1) {
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

			if (S_ISDIR(st.st_mode) && nftw(*argv, wchgrp, 17, call.flags) == -1) {
				func_file_error("nftw", *argv);
				ret = 1;
			}
		}
	}

	_exit(ret);
}
