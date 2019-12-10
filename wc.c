#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/uio.h>

#include <locale.h>

#include <wctype.h>

#include "io.h"

#define FLAGSSTR " [-c | -m] [-lw] [file...]\n"

enum {lFLAG, mFLAG, cFLAG, wFLAG, FLAGSIZE};
static unsigned flags[FLAGSIZE];

size_t tbytes, tnewline, twords;

static void
usage(void)
{
	USAGEBEG {
		USAGESTR, STRLEN(USAGESTR),
		argv0, strlen(argv0),
		FLAGSSTR, STRLEN(FLAGSSTR)
	} USAGEEND
}

static void
write_stats(size_t newline, size_t words, size_t bytes, const char *name)
{
	if (flags[lFLAG]) {
		write_long(1, newline, 1);
		str_write_buf(1, " ");
	}

	if (flags[wFLAG]) {
		write_long(1, words, 1);
		str_write_buf(1, " ");
	}

	if (flags[cFLAG] || flags[mFLAG]) {
		write_long(1, bytes, 1);
		str_write_buf(1, " ");
	}

	if (name)
		str_write_buf(1, name);
	str_write_buf(1, "\n");
}

static int
mbtoc(wchar_t *w, const char *buf, size_t n)
{
	int ret = 1;

	*w = 0;
	if (n)
		*w = *buf;
	else
		ret = 0;

	return ret;
}

static void
wc(int fd, const char *name)
{
	size_t bytes = 0, chars = 0, newline = 0, words = 0, prev = 0;
	ssize_t r;
	char buf[BUFSIZ], *ptr;
	int word = 0;

	while (r = read(fd, buf + prev, sizeof(buf) - prev)) {
		if (r == -1)
			func_error_exit("read");

		ptr = buf;
		bytes += r;
		r += prev;
		/* ensures incomplete characters are saved until the end */
		while (r >= MB_CUR_MAX) {
			wchar_t tmp = 0;
			int i, next = flags[cFLAG] ? mbtoc(&tmp, ptr, r) : mbtowc(&tmp, ptr, r);
			if (next == -1) {
				--r;
				++ptr;
				if (word)
					++words;
				word = 0;
				continue;
			} else if (!next) {
				if (word)
					++words;
				word = 0;
				next = 1;
			}

			newline += tmp == '\n';
			for (i = 0; i < next; ++i) {
				char *p = (char *) &tmp;
			 	if (iswspace(p[i])) {
					if (word)
						++words;
					word = 0;
				} else
					word = 1;
			}

			ptr += next;
			r -= next;
			if (flags[mFLAG])
				++chars;
		}

		prev = r;
		memcpy(buf, ptr, r);
	}

	/* check if last character is complete. */
	ptr = buf;
	while (prev) {
		wchar_t tmp = 0;
		int i, next = flags[cFLAG] ? mbtoc(&tmp, ptr, prev) : mbtowc(&tmp, ptr, prev);
		if (next == -1) {
			--prev;
			++ptr;
			if (word)
				++words;
				word = 0;
				continue;
			} else if (!next) {
				if (word)
					++words;
				word = 0;
				next = 1;
			}

		newline += tmp == '\n';
		for (i = 0; i < next; ++i) {
			char *p = (char *) &tmp;
		 	if (isspace(p[i])) {
				if (word)
					++words;
				word = 0;
			} else
				word = 1;
		}

		ptr += next;
		prev -= next;
		if (flags[mFLAG])
			++chars;
	}

	write_stats(newline, words, flags[cFLAG] ? bytes : chars, name);
	tnewline += newline;
	twords += words;
	tbytes += flags[cFLAG] ? bytes : chars;
}

int
main(int argc, char *argv[])
{
	int fd, ret = 0;

	ARGBEG("clmw") {
	case 'c':
		flags[cFLAG] = 1;
		flags[mFLAG] = 0;
		break;
	case 'l':
		flags[lFLAG] = 1;
		break;
	case 'm':
		flags[mFLAG] = 1;
		flags[cFLAG] = 0;
		break;
	case 'w':
		flags[wFLAG] = 1;
		break;
	default:
		usage();
	} ARGEND

	if (!(flags[cFLAG] + flags[mFLAG] + flags[lFLAG] + flags[wFLAG]))
		flags[cFLAG] = flags[lFLAG] = flags[wFLAG] = 1;

	//if (flags[mFLAG])
		setlocale(LC_CTYPE, "");

	for (; *argv; ++argv) {
		if (**argv != '-' || *(*argv+1) != '\0') {
			fd = open(*argv, O_RDONLY);
			if (fd == -1) {
				ret = 1;
				continue;
			}
		} else
			fd = 0;

		wc(fd, fd ? *argv : NULL);

		if (fd)
			close(fd);
	}

	if (argc-optind > 1)
		write_stats(tnewline, twords, tbytes, "total");
	str_write_flush(1);

	_exit(ret);
}
