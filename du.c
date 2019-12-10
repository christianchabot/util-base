#include <dirent.h>
#include <errno.h>
#include <ftw.h>
#include <search.h>
#include <stdlib.h>
#include <string.h>
#include <sys/statvfs.h>
#include <unistd.h>

/* should not follow symbolic links by default */
#include "ch.h"
#include "io.h"

#define FLAGSSTR " [-a | -s] [-kx] [-H | -L] [file...]\n"

enum {HFLAG, LFLAG, aFLAG, kFLAG, sFLAG, xFLAG, FLAGSIZE};
static size_t blksiz = 512, total;
static unsigned flags[FLAGSIZE];
static int ret;

static int
wdu(const char *path, const struct stat *st, int info, struct FTW *ftw)
{
	struct statvfs vst;

	if (!flags[aFLAG] && !(info & FTW_D))
		return 0;

	/* if the x flag is not set then this makes it more difficult. */
	/* get filesystem rooted at given directory first */
	if (statvfs(path, &vst) == -1) {
		func_file_error("statvfs", path);
		ret = 1;
		return 0;
	}

	/* too much memory usage for lots of files */
	/* ******** todo work on this ********* */
	/*
	if (hcreate(vst.f_files) == -1) {
		func_error("hcreate");
		ret = 1;
		return 0;
	}
	*/

	total += st->st_blocks; /* / blksiz; */
	write_strings(NULL, static_long_string(st->st_blocks), " ", path, "\n", NULL);

	/* if st_nlink > 1 hash it in table */
	if (st->st_nlink > 1); /* do nothing right now */
		/* should hash dev and ino pairs for files with hardlinks > 1 */

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
	char *tmp;

	ARGBEG("aHkLsx") {
	case 'a':
		flags[aFLAG] = 1;
		call.flags |= FTW_DEPTH;
		break;
	case 'H':
		flags[HFLAG] = 1;
		call = funcs[HFLAG];
		break;
	case 'k':
		flags[kFLAG] = 1;
		break;
	case 'L':
		flags[LFLAG] = 1;
		call = funcs[LFLAG];
		break;
	case 's':
		flags[sFLAG] = 1;
		break;
	case 'x':
		flags[xFLAG] = 1;
		call.flags |= FTW_MOUNT;
		break;
	default:
		usage();
	} ARGEND

	if (((flags[aFLAG] + flags[sFLAG]) | (flags[HFLAG] + flags[LFLAG])) > 1)
		usage();

	call.flags &= ~FTW_DEPTH;
	if (!flags[kFLAG]) {
		char *eblksiz = getenv("BLOCKSIZE");
		if (eblksiz) {
			long tmp = strtol(eblksiz, NULL, 10);
			if (tmp <= 0 || errno)
				name_errors_exit("invalid value for BLOCKSIZE in environment: ", eblksiz, "\n", NULL);

			/* check this */
			if (tmp > 512)
				blksiz = tmp;
		}
	} else
		blksiz = 1024;

	/* use nftw */
	if (argc - optind < 1)
		if (nftw(".", wdu, 17, call.flags) == -1) {
			func_file_error("nftw", ".");
			ret = 1;
		}

	while (tmp = *argv++) {
		/* Check if directory and make sure to print entries in the directory and not the directory itself. This is how the suckless du and gnu du does it. */
		char *lstr;
		struct dirent *d;
		struct stat st = {0};

		if (lstat(tmp, &st) == -1) {
			func_file_error("lstat", tmp);
			ret = 1;
		}

		if (S_ISDIR(st.st_mode)) {
			struct dirent *d;
			DIR *dp = opendir(tmp);
			if (!dp) {
				func_file_error("opendir", tmp);
				ret = 1;
			} else
				while (d = readdir(dp))
					if (strcmp("..", d->d_name) && strcmp(".", d->d_name))
						if (nftw(d->d_name, wdu, 17, call.flags) == -1) {
							func_file_error("nftw", d->d_name);
							ret = 1;
						}
		}

		/* fix this */
		total += st.st_blocks;
		lstr = total ? static_long_string(total) : NULL;
		write_strings(NULL, lstr, " ", tmp, "\n", NULL);
	}
	write_strings(ioflush, NULL);

	/*
	tmp = total ? static_long_string(total) : NULL;
	write_strings(ioflush, tmp, " total\n", NULL);
	*/

	_exit(ret);
}
