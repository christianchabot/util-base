#include <ftw.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/uio.h>

#include "io.h"
#include "mode.h"

#define FLAGSSTR " [-R] mode file...\n"

static unsigned Rflag;
static int ret;
static mode_t mode;

static int
wchmod(const char *path, const struct stat *st, int info, struct FTW *ftw)
{
	if (chmod(path + ftw->base, mode) == -1) {
		func_file_error("chmod", path + ftw->base);
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
	ARGBEG("R") {
	case 'R':
		Rflag = 1;
		break;
	default:
		usage();
	} ARGEND

	if (argc-optind < 2)
		usage();

	mode = parsemode(*argv, umask(0));
	if (mode == -1)
		name_errors_exit("invalid mode: ", *argv, "\n", NULL);

	while (*++argv) {
		if (chmod(*argv, mode) == -1) {
			func_file_error("chmod", *argv);
			ret = 1;
			continue;
		}

		if (Rflag) {
			struct stat st;

			if (stat(*argv, &st) == -1) {
				func_file_error("stat", *argv);
				ret = 1;
				continue;
			}

			if (S_ISDIR(st.st_mode) && nftw(*argv, wchmod, 17, FTW_CHDIR | FTW_PHYS) == -1) {
				func_file_error("nftw", *argv);
				ret = 1;
			}
		}
	}

	_exit(ret);
}
