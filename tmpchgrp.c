#include <errno.h>
#include <grp.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "io.h"
#include "path.h"

#define FLAGSSTR " [-h] [-R [-H | -L | -P]] group file...\n"\

struct ids {
	int (*chown) (const char *, uid_t, gid_t);
	char *cname;
	gid_t gid;
};

#define TOSTR(x) #x
#define INIT(c, rc, s, rs) c, rc, s, rs, TOSTR(c), TOSTR(rc), TOSTR(s), TOSTR(rs)

struct call {
	int (*chown) (const char *, uid_t, gid_t), (*rchown) (const char *, uid_t, gid_t);
	int (*stat) (const char *, struct stat *), (*rstat) (const char *, struct stat *);
	char *cname, *rcname, *sname, *rsname;
} calls[] = {
	INIT(lchown, chown, stat, stat),
	INIT(chown, lchown, stat, lstat),
	INIT(chown, chown, stat, stat),
	INIT(lchown, lchown, lstat, lstat)
};

#undef INIT
#undef TOSTR

enum {hFLAG, HFLAG, LFLAG, PFLAG, RFLAG, FLAGSIZE};
static unsigned flags[FLAGSIZE];
static int ret;

static void
wchgrp(const char *path, void *args)
{
	struct ids *ids = args;

	if (ids->chown(path, -1, ids->gid) == -1) {
		func_file_error(ids->cname, path);
		ret = 1;
	}
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
	struct call *call = calls + LFLAG;
	struct group *grp;
	struct ids ids = {chown, "chown", -1};
	gid_t gid;

	ARGBEG("hHLPR") {
	case 'h':
		flags[hFLAG] = 1;
		call = calls + hFLAG;
		break;
	case 'H':
		flags[HFLAG] = 1;
		call = calls + HFLAG;
		break;
	case 'L':
		flags[LFLAG] = 1;
		call = calls + LFLAG;
		break;
	case 'P':
		flags[PFLAG] = 1;
		call = calls + PFLAG;
		break;
	case 'R':
		flags[RFLAG] = 1;
		break;
	default:
		usage();
	} ARGEND

	if (argc-optind < 2 || flags[hFLAG] + flags[RFLAG] > 2)
		usage();
	else if (!flags[RFLAG] && (flags[HFLAG] || flags[LFLAG] || flags[PFLAG]))
		usage();

	grp = getgrnam(*argv);
	if (!grp) {
		gid = strtol(*argv, NULL, 10);
		if (!gid && errno)
			name_errors_exit("invalid group: ", *argv, "\n", NULL);

		grp = getgrgid(gid);
		if (!grp)
			if (errno)
				func_error_exit("getgrgid");
			else
				name_errors_exit("invalid group: ", *argv, "\n", NULL);
	}

	ids.chown = call->rchown;
	ids.cname = call->rcname;
	ids.gid = grp->gr_gid;
	while (*++argv) {
		if (call->chown(*argv, -1, ids.gid) == -1) {
			func_file_error(call->cname, *argv);
			ret = 1;
			continue;
		}

		if (flags[RFLAG]) {
			struct stat st;

			if (call->stat(*argv, &st) == -1) {
				func_error(call->sname);
				ret = 1;
				continue;
			}

			if (S_ISDIR(st.st_mode))
				recurse(*argv, call->rstat, call->rsname, &ids, wchgrp);
		}
	}

	_exit(ret);
}
