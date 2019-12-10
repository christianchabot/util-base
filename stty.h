#ifndef STTY_H_
#define STTY_H_

#define TOSTR(x) #x
#define VALSTR(x) TOSTR(x)
#define MODE(x, y) {x, VALSTR(y), y}

static struct mode {
	char *name, *val;
	long mask; /* Largest object value that can represent all baud rates and flags. */
} baud[] = {
	MODE("ispeed", -1), MODE("ospeed", -1), MODE("0", B0), MODE("50", B50), MODE("75", B75),
	MODE("110", B110), MODE("134", B134), MODE("150", B150), MODE("200", B200), MODE("300", B300),
	MODE("600", B600), MODE("1200", B1200), MODE("1800", B1800), MODE("2400", B2400),
	MODE("4800", B4800), MODE("9600", B9600), MODE("19200", B19200), MODE("38400", B38400),
#ifdef B57600
	MODE("57600", B57600),
#endif
#ifdef B115200
	MODE("115200", B115200),
#endif
#ifdef B230400
	MODE("230400", B230400),
#endif
#ifdef B460800
	MODE("460800", B460800),
#endif
#ifdef B921600
	MODE("921600", B921600),
#endif
}, cmode[] = {
	MODE("parenb", PARENB), MODE("parodd", PARODD), MODE("hupcl", HUPCL),
	MODE("cstopb", CSTOPB), MODE("cread", CREAD), MODE("clocal", CLOCAL)
}, imode[] = {
	MODE("ignbrk", IGNBRK), MODE("brkint", BRKINT), MODE("ignpar", IGNPAR), MODE("parmrk", PARMRK),
	MODE("inpck", INPCK), MODE("istrip", ISTRIP), MODE("inlcr", INLCR), MODE("ixon", IXON),
	MODE("ixany", IXANY), MODE("ixoff", IXOFF)
}, omode[] = {
	MODE("opost", OPOST), MODE("onlcr", ONLCR), MODE("ocrnl", OCRNL), MODE("onocr", ONOCR),
	MODE("nlret", ONLRET), MODE("ofill", OFILL), MODE("ofdel", OFDEL)
}, lmode[] = {
	MODE("isig", ISIG), MODE("icanon", ICANON), MODE("iexten", IEXTEN), MODE("echo", ECHO),
	MODE("echoe", ECHOE), MODE("echok", ECHOK), MODE("echonl", ECHONL), MODE("noflsh", NOFLSH),
	MODE("tostop", TOSTOP)
}, charsize[] = {
	MODE("cs8", CS8), MODE("cs5", CS5), MODE("cs6", CS6), MODE("cs7", CS7)
}, crdelay[] = {
	MODE("cr0", CR0), MODE("cr1", CR1), MODE("cr2", CR2), MODE("cr3", CR3)
}, nldelay[] = {
	MODE("nl0", NL0), MODE("nl1", NL1)
}, htdelay[] = {
	MODE("tab0", TAB0), MODE("tab1", TAB1), MODE("tab2", TAB2), MODE("tab3", TAB3)
}, bsdelay[] = {
	MODE("bs0", BS0), MODE("bs1", BS1)
}, ffdelay[] = {
	MODE("ff0", FF0), MODE("ff1", FF1)
}, vtdelay[] = {
	MODE("vt0", VT0), MODE("vt1", VT1)
}, controlc[] = {
	MODE("eof", VEOF), MODE("eol", VEOL), MODE("erase", VERASE), MODE("intr", VINTR),
	MODE("kill", VKILL), MODE("quit", VQUIT), MODE("susp", VSUSP), MODE("start", VSTART),
	MODE("stop", VSTOP),
#ifdef VDISCARD
	MODE("discard", VDISCARD),
#endif
#ifdef VDSUSP
	MODE("dsusp", VDSUSP),
#endif
#ifdef VEOL2
	MODE("eol2", VEOL2),
#endif
#ifdef VERASE2
	MODE("erase2", VERASE2),
#endif
#ifdef VLNEXT
	MODE("lnext", VLNEXT),
#endif
#ifdef VREPRINT
	MODE("reprint", VREPRINT),
#endif
#ifdef VSTATUS
	MODE("status", VSTATUS),
#endif
#ifdef VWERASE
	MODE("werase", VWERASE),
#endif
#ifdef VSWTCH
	MODE("swtch", VSWTCH),
#endif
}, extra[] = {
	MODE("hup", HUPCL), MODE("ispeed", -1), MODE("ospeed", -1), MODE("min", -1), MODE("time", -1),
	MODE("evenp", -1), MODE("parity", -1), MODE("oddp", -1), MODE("raw", -1), MODE("cooked", -1),
	MODE("nl", -1), MODE("ek", -1), MODE("sane", -1)
};

#endif
