#include <string.h>
#include <syslog.h>
#include <sys/uio.h>

#define FLAGSSTR " string...\n"

static void
usage(void)
{
	USAGEBEG {
		USAGESTR, STRLEN(USAGESTR)
		argv0, strlen(argv0),
		FLAGSSTR, STRLEN(FLAGSSTR)
	} USAGEEND
}

int
main(int argc, char *argv[])
{
	
	_exit(0);
}
