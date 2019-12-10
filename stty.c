#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/uio.h>
#include <termios.h>
#include <unistd.h>

#ifndef TIOCGWINSZ
#include <sys/ioctl.h>
# ifdef TIOCGWINSZ /* if sys/ioctl.h has TIOCGWINSZ then need limits also. */
#include <limits.h>
# endif
#endif

#include "stty.h"
#include "io.h"

/* **************** TODO ****************
	Add non posix terminal options. Use linux man and advanced unix programming book.
*/

#define FLAGSSTR " [-a | -g | operand...]\n"

static void
dump_dimen(void)
{
	char *tmp;
#ifdef TIOCGWINSZ
	struct winsize ws;

	if (ioctl(1, TIOCGWINSZ, &ws) != -1) {
		char col[STRLEN(VALSTR(INT_MAX))], row[STRLEN(VALSTR(INT_MAX))];
		sprintf(row, "%d", ws.ws_row);
		str_write_buf(1, "rows ");
		str_write_buf(1, row);
		sprintf(col, "%d", ws.ws_col);
		str_write_buf(1, "; columns ");
		str_write_buf(1, col);
		str_write_buf(1, ";");
	} else {
#endif
	tmp = getenv("ROWS");
	if (tmp) {
		str_write_buf(1, "rows ");
		str_write_buf(1, tmp);
		str_write_buf(1, "; ");
	}
	tmp = getenv("COLUMNS");
	if (tmp) {
		str_write_buf(1, "columns ");
		str_write_buf(1, tmp);
		str_write_buf(1, ";");
	}
#ifdef TIOCGWINSZ
	}
#endif
}

#undef MODE
#undef VALSTR
#undef TOSTR

static void
dump_speed(struct termios *t)
{
	speed_t ispeed = cfgetispeed(t), ospeed = cfgetospeed(t);
	size_t i;

	if (ispeed != ospeed)
		str_write_buf(1, "i");

	str_write_buf(1, "speed ");
	for (i = 0; i < ARRSIZ(baud); ++i)
		if (ispeed == baud[i].mask) {
			str_write_buf(1, baud[i].name);
			break;
		}

	if (ispeed != ospeed) {
		str_write_buf(1, " baud; ospeed ");
		for (i = 0; i < ARRSIZ(baud); ++i)
			if (ospeed == baud[i].mask) {
				str_write_buf(1, baud[i].name);
				break;
			}
	}
	str_write_buf(1, " baud; ");
	dump_dimen();
	str_write_buf(1, "\n");
}

static void
dump_modes(struct termios *t)
{
	struct {
		struct mode *mode;
		size_t len;
		tcflag_t flag;
	} modes[] = {
		cmode, ARRSIZ(cmode), t->c_cflag,
		imode, ARRSIZ(imode), t->c_iflag,
		omode, ARRSIZ(omode), t->c_oflag,
		lmode, ARRSIZ(lmode), t->c_lflag
	};
	size_t i, j;

	for (i = 0; i < ARRSIZ(modes); ++i) {
		for (j = 0; j < modes[i].len; ++j) {
			if (modes[i].mode[j].mask & modes[i].flag)
				str_write_buf(1, modes[i].mode[j].name);
			else {
				str_write_buf(1, "-");
				str_write_buf(1, modes[i].mode[j].name);
			}
			str_write_buf(1, " ");
		}
		str_write_buf(1, "\n");
	}
}


/* ******************** Might be able to change this function and make it smaller. ****************** */
static void
dump_delays(struct termios *t)
{
	char *tmp = "";

	switch (t->c_cflag & CSIZE) {
	case CS5:
		tmp = "cs5 ";
		break;
	case CS6:
		tmp = "cs6 ";
		break;
	case CS7:
		tmp = "cs7 ";
		break;
	case CS8:
		tmp = "cs8 ";
		break;
	}
	str_write_buf(1, tmp);

	tmp = "";
	switch (t->c_oflag & CRDLY) {
	case CR0:
		tmp = "cr0 ";
		break;
	case CR1:
		tmp = "cr1 ";
		break;
	case CR2:
		tmp = "cr2 ";
		break;
	case CR3:
		tmp = "cr3 ";
		break;
	}
	str_write_buf(1, tmp);

	tmp = "";
	switch (t->c_oflag & NLDLY) {
	case NL0:
		tmp = "nl0 ";
		break;
	case NL1:
		tmp = "nl1 ";
		break;
	}
	str_write_buf(1, tmp);

	tmp = "";
	switch (t->c_oflag & TABDLY) {
	case TAB0:
		tmp = "tab0 ";
		break;
	case TAB1:
		tmp = "tab1 ";
		break;
	case TAB2:
		tmp = "tab2 ";
		break;
	case TAB3:
		tmp = "tab3 ";
		break;
	}
	str_write_buf(1, tmp);

	tmp = "";
	switch (t->c_oflag & BSDLY) {
	case BS0:
		tmp = "bs0 ";
		break;
	case BS1:
		tmp = "bs1 ";
		break;
	}
	str_write_buf(1, tmp);

	tmp = "";
	switch (t->c_oflag & FFDLY) {
	case FF0:
		tmp = "ff0 ";
		break;
	case FF1:
		tmp = "ff1 ";
		break;
	}
	str_write_buf(1, tmp);

	tmp = "";
	switch (t->c_oflag & VTDLY) {
	case VT0:
		tmp = "vt0 ";
		break;
	case VT1:
		tmp = "vt1 ";
		break;
	}
	str_write_buf(1, tmp);
	str_write_buf(1, "\n");
}

static void
dump_controls(struct termios *t)
{
	size_t i;
	char buf[] = "^\0";

	for (i = 0; i < ARRSIZ(controlc); ++i) {
		str_write_buf(1, controlc[i].name);
		str_write_buf(1, " = ");
		if (t->c_cc[controlc[i].mask] + '@' == '@')
			str_write_buf(1, "undef");
		else {
			buf[1] = t->c_cc[controlc[i].mask] == 127 ? '?' : t->c_cc[controlc[i].mask] + '@';
			str_write_buf(1, buf);
		}
		str_write_buf(1, "; ");
	}
	str_write_buf(1, "\n");
}

static void
dump_settings(struct termios *t)
{
	dump_speed(t);
	dump_modes(t);
	dump_delays(t);
	dump_controls(t);
	str_write_flush(1);
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
	struct termios t;
	int ret = 0;

	ARGBEG("ag") {
	case 'a':
		if (tcgetattr(0, &t) == -1)
			func_error_exit("tcgetattr");
		dump_settings(&t);
		_exit(0);
	case 'g':
		break;
	default:
		usage();
	} ARGEND

	if (argc - optind < 1) {
		if (tcgetattr(0, &t) == -1)
			func_error_exit("tcgetattr");
		dump_speed(&t);
		str_write_flush(1);
	}

	/* else start setting the values of the terminal. */

	_exit(ret);
}
