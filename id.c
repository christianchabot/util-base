#include <errno.h>
#include <grp.h>
#include <limits.h>
#include <pwd.h>
#include <stddef.h>
#include <string.h>
#include <sys/uio.h>
#include <unistd.h>

#include <stdio.h>

#include "io.h"

#define FLAGSSTR " [user | -G [-n] user | -g [-nr] user | -u [-nr] user]\n"

enum {GFLAG, gFLAG, nFLAG, rFLAG, uFLAG, FLAGSIZE};
static unsigned flags[FLAGSIZE];

static void
usage(void)
{
	USAGEBEG {
		USAGESTR, STRLEN(USAGESTR),
		argv0, strlen(argv0),
		FLAGSSTR, STRLEN(FLAGSSTR)
	} USAGEEND
}

#define TOSTR(x) #x
#define TOVAL(x) TOSTR(x)

static struct group *
getnextsupplementary(struct passwd *pwd)
{
	struct group *grp;

	errno = 0;
	while (grp = getgrent()) {
		for (; *grp->gr_mem; ++(grp->gr_mem))
			if (!strcmp(*grp->gr_mem, pwd->pw_name))
				return grp;
		errno = 0;
	}

	if (errno)
		func_error_exit("getgrent");
	else
		return NULL;
}

static struct group *
xgetgrgid(gid_t gid)
{
	struct group *grp = getgrgid(gid);

	if (!grp)
		if (errno)
			func_error_exit("getgrgid");
		else
			name_errors_exit("group entry not found\n", NULL);
	return grp;
}

static void
user(struct passwd *pwd)
{
	if (flags[nFLAG])
		write_strings(NULL, pwd->pw_name);
	else
		write_strings(NULL, static_long_string(pwd->pw_uid), NULL);
}

static void
group(struct group *grp)
{
	if (flags[nFLAG])
		write_strings(NULL, grp->gr_name);
	else
		write_strings(NULL, static_long_string(grp->gr_gid), NULL);
}

static void
groups(struct passwd *pwd)
{
	struct group *grp;

	if (flags[nFLAG])
		write_strings(NULL, xgetgrgid(pwd->pw_gid)->gr_name, NULL);
	else
		write_strings(NULL, static_long_string(pwd->pw_gid), NULL);
	
	while (grp = getnextsupplementary(pwd)) {
		write_strings(NULL, " ", NULL);
		group(grp);
	}
}

static void
print_format(const char *str, id_t id, const char *name)
{
	write_strings(NULL, str, static_long_string(id), "(", name, ")", NULL);
}

#undef TOVAL
#undef TOSTR

static void
default_print(struct passwd *pwd)
{
	struct group *grp = xgetgrgid(pwd->pw_gid);
	uid_t euid = geteuid();

	print_format("uid=", pwd->pw_uid, pwd->pw_name);
	print_format(" gid=", grp->gr_gid, grp->gr_name);
	if (pwd->pw_uid != euid) {
		struct passwd *tmp = getpwuid(euid);
		if (tmp)
			print_format(" euid=", euid, tmp->pw_name);
	}

	print_format(" groups=", grp->gr_gid, grp->gr_name);
	while (grp = getnextsupplementary(pwd))
		print_format(",", grp->gr_gid, grp->gr_name);
}

static void
id(const char *name)
{
	struct passwd *pwd;

	if (name)
		pwd = getpwnam(name);
	else
		pwd = getpwuid(getuid());

	if (!pwd)
		if (errno)
			func_error_exit(name ? "getpwname" : "getpwuid");
		else
			name_errors_exit("user entry not found\n", NULL);

	setgrent();
	if (flags[GFLAG])
		groups(pwd);
	else if (flags[gFLAG])
		group(xgetgrgid(pwd->pw_gid));
	else if (flags[uFLAG])
		user(pwd);
	else
		default_print(pwd);
	endgrent();
	write_strings(ioflush, "\n", NULL);
}

int
main(int argc, char *argv[])
{
	ARGBEG("Ggnru") {
	case 'G':
		flags[GFLAG] = 1;
		break;
	case 'g':
		flags[gFLAG] = 1;
		break;
	case 'n':
		flags[nFLAG] = 1;
		break;
	case 'r':
		flags[rFLAG] = 1;
		break;
	case 'u':
		flags[uFLAG] = 1;
		break;
	default:
		usage();
	} ARGEND

	if (argc - optind > 1 || flags[GFLAG] + flags[gFLAG] + flags[uFLAG] > 1)
		usage();

	id(*argv);
	_exit(0);
}
