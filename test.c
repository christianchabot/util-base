#include <string.h>

#define FLAGSSTR " [expression]"

static char *end_arg;

static void
usage(void)
{
	if (!strcmp(argv0, "[")) {
		USAGEBEG {
			USAGESTR, STRLEN(USAGESTR),
			argv0, strlen(argv0),
			" [", STRLEN(" ["),
			FLAGSSTR, STRLEN(FLAGSSTR)
			" ]\n", STRLEN(" ]\n")
		} USAGEEND
	} else {
		USAGEBEG {
			USAGESTR, STRLEN(USAGESTR),
			argv0, strlen(argv0),
			FLAGSSTR, STRLEN(FLAGSSTR)
			"\n", STRLEN("\n")
		} USAGEEND
	}
}

int
main(int argc, char *argv[])
{
	int ret = 0;

	argv0 = *argv, argv0 ? (--argc, ++argv) : (void *) 0;
	if (argv0 && !strcmp(argv0, "["))
		end_arg = "]";


	for (; *argv; ++argv) {
		if (end_arg && !strcmp(*argv, end_arg))
			_exit(ret);

		
	}

	_exit(ret);
}
