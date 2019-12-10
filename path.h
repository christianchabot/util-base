#ifndef PATH_H_
#define PATH_H_

#include <sys/stat.h>

int recurse(const char *, int (*) (const char *, struct stat *), char *, void *, void (*) (const char *, void *));
int mkparentdir(char *, mode_t);

#endif
