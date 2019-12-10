#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/uio.h>

#include "io.h"
#include "mode.h"
#include "path.h"

#define FLAGSSTR " [-p] [-m mode] dir...\n"

static unsigned pflag;

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
	int ret = 0;
	mode_t mode = S_IRWXU | S_IRWXG | S_IRWXO;

	ARGBEG("pm:") {
	case 'p':
		pflag = 1;
		break;
	case 'm':
		mode = parsemode(optarg, mode);
		if (mode == -1)
			name_errors_exit("invalid mode argument: ", optarg, "\n", NULL);
		break;
	default:
		usage();
	} ARGEND

	if (argc-optind < 1)
		usage();

	mode &= ~umask(umask(0));
	for (; *argv; ++argv)
		if (pflag) {
			if (mkparentdir(*argv, mode) == -1)
				ret = 1;
		} else {
			if (mkdir(*argv, mode) == -1)
				if (errno == EEXIST) {
					struct stat st;

					if (stat(*argv, &st) == -1) {
						func_error("stat");
						ret = 1;
					} else
						if (!S_ISDIR(st.st_mode)) {
							func_file_error("mkdir", *argv);
							ret = 1;
						}
				} else {
					func_file_error("mkdir", *argv);
					ret = 1;
				}
		}

	_exit(ret);
}
