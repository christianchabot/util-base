#ifndef IO_H_
#define IO_H_

#include <sys/uio.h>
#include <unistd.h>

#include "err.h"

#define USAGESTR "usage: "
#define USAGEALN "       "

#define ARRSIZ(x) (sizeof(x)/sizeof(*(x)))
#define STRLEN(x) (sizeof((x))-1)

#define ARGBEG(str) {\
		extern char *optarg;\
		extern int optind, optopt, opterr;\
		int c;\
		argv0 = *argv; opterr = 0;\
		while ((c = getopt(argc, argv, (str))) != -1)\
			switch (c)

#define ARGEND }\
		argv+=optind;

#define IOV iov

#define IOVBEG \
		size_t i;\
		struct iovect iovt[] =

#define IOVEND ;\
		struct iovec IOV[sizeof(iovt)/sizeof(*iovt)];\
		for (i = 0; i < sizeof(iovt)/sizeof(*iovt); ++i)\
			IOV[i].iov_base = iovt[i].base, IOV[i].iov_len = iovt[i].len;

#define USAGEBEG IOVBEG

#define USAGEEND IOVEND\
		writevn(2, IOV, ARRSIZ(IOV));\
		_exit(1);

struct iovect {
	void *base;
	size_t len;
};

char *argv0;

ssize_t readn(int, void *, size_t);
ssize_t writen(int, const void *, size_t);
ssize_t writevn(int, struct iovec *, int);
void err_exit(int);
void strerr_exit(char *, int);
ssize_t rdfd_wrout(int);
ssize_t rdfd_wrout_buf(int);
void rdwrout_flush(void);
ssize_t ioflush(int, void *, size_t);
void write_buffers(struct iovec *iov, size_t iovcnt, ssize_t (*) (int, void *, size_t));
void write_strings(ssize_t (*) (int, void *, size_t), ...);
void str_write_buf(int, const char *);
void str_write_flush(int);
void write_long(int, long, int);
char *static_long_string(long);

#endif /* IO_H_ */
