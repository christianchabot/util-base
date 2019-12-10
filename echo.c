#include <ctype.h>
#include <string.h>

#include "io.h"
#include "convert.h"

static size_t 
slashchar(const char *buf, char **c)
{
	static char oth[] = {'\\', 0, '\0'}, *oct;
	size_t r = 2;

	if (*buf++ == '\\') {
		switch (*buf) {
		case 'a': *c = "\a"; break;
		case 'b': *c = "\b"; break;
		case 'c': *c = NULL; break;
		case 'f': *c = "\f"; break;
		case 'n': *c = "\n"; break;
		case 'r': *c = "\r"; break;
		case 't': *c = "\t"; break;
		case 'v': *c = "\v"; break;
		case '\\': *c = "\\"; break;
		case '0':
			/* r += octstr_to_binstr(buf+1, c); */ /* Validate the implementation */
			r += octstr_to_bin(buf+1, oth);
			*c = oth;
			break;
		case '\0':
			r = 1;
		default:
			*(oth+1) = *buf;
			*c = oth;
			break;
		}
	} else
		r = 1;

	return r;
}

static void
echo(char *str)
{
	char *prev = str, *slash, **tmp;
	size_t r;

	while (slash = strchr(prev, '\\')) {
		r = slashchar(slash, tmp);
		*slash = '\0';
		if (!*tmp) {
			write_strings(ioflush, prev, NULL);
			_exit(0);
		}

		write_strings(NULL, prev, *tmp, NULL);
		prev = slash + r;
	}
	write_strings(NULL, prev, NULL);
}

int
main(int argc, char *argv[])
{
	char *slash;

	argv0 = *argv, argv0 ? (--argc, ++argv) : (void *) 0;

	while (*argv)
		echo(*argv++);

	write_strings(ioflush, "\n", NULL);
	_exit(0);
}
