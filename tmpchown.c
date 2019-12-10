#include <errno.h>
#include <grp.h>
#include <pwd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "io.h"
#include "path.h"

#define FLAGSSTR1 " [-h] owner[:group] file...\n"
#define FLAGSSTR2 " -R [-H | -L | -P] owner[:group] file...\n"

struct ids {
	int (*chown) (const char *, uid_t, gid_t);
	char *cname;
	uid_t uid;
	gid_t gid;
};

#define TOSTR(x) #x
#define INIT(c, rc, s, rs) c, rc, s, rs, TOSTR(c), TOSTR(rc), TOSTR(s), TOSTR(rs)

struct call {
	int (*chown) (const char *, uid_t, gid_t), (*rchown) (const char *, uid_t, gid_t);
	int (*stat) (const char *, struct stat *), (*rstat) (const char *, struct stat *);
	char *cname, *rcname, *sname, *rsname;
} calls[] = {
	INIT(lchown, lchown, stat, stat),
	INIT(chown, lchown, stat, lstat),
	INIT(chown, chown, stat, stat),
	INIT(lchown, lchown, lstat, lstat),
};

#undef INIT
#undef TOSTR

enum {hFLAG, HFLAG, LFLAG, PFLAG, RFLAG, FLAGSIZE};
static unsigned flags[FLAGSIZE];
static int ret;

static void
wchown(const char *path, void *args)
{
	struct ids *ids = args;

	if (ids->chown(path, ids->uid, ids->gid) == -1) {
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
		FLAGSSTR1, STRLEN(FLAGSSTR1),
		USAGEALN, STRLEN(USAGEALN),
		argv0, strlen(argv0),
		FLAGSSTR2, STRLEN(FLAGSSTR2)
	} USAGEEND
}

int
main(int argc, char *argv[])
{
	struct call *call = calls + LFLAG;
	struct passwd *pw;
	char *group;
	struct ids ids = {chown, "chown", -1, -1};

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

	if (argc-optind < 2 || flags[hFLAG] + flags[RFLAG] > 1)
		usage();
	else if (!flags[RFLAG] && (flags[HFLAG] || flags[PFLAG] || flags[LFLAG]))
		usage();

	if (group = strchr(*argv, ':'))
		*group++ = '\0';

	pw = getpwnam(*argv);
	if (!pw) {
		if (errno) 
			func_error_exit("getpwnam");

		ids.uid = strtol(*argv, NULL, 10);
	 	if (!ids.uid && errno)
			name_errors_exit("invalid owner: ", *argv, "\n", NULL);

		pw = getpwuid(ids.uid);
		if (!pw)
			if (errno)
				func_error_exit("getpwuid");
			else
				name_errors_exit("invalid owner: ", *argv, "\n", NULL);
	}
	ids.uid = pw->pw_uid;

	if (group) {
		struct group *gr = getgrnam(group);
		if (!gr) {
			if (errno)
				func_error_exit("getgrnam");

			ids.gid = strtol(*argv, NULL, 10);
			if (!ids.gid && errno)
				name_errors_exit("invalid group: ", group, "\n", NULL);

			gr = getgrgid(ids.gid);
			if (!gr)
				if (errno)
					func_error_exit("getgrgid");
				else
					name_errors_exit("invalid group: ", *argv, "\n", NULL);
		}
		ids.gid = gr->gr_gid;
	}

	ids.chown = call->rchown;
	ids.cname = call->rcname;
	while (*++argv) {
		if (call->chown(*argv, ids.uid, ids.gid) == -1) {
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

			if (S_ISDIR(st.st_mode) && recurse(*argv, call->rstat, call->rsname, &ids, wchown) == -1)
				ret = 1;
		}
	}

	_exit(ret);
}
