#ifndef CH_H_
#define CH_H_

#include <unistd.h>

#define TOSTR(x) #x
#define INIT(st, ch, rch, flags) {st, ch, rch, TOSTR(st), TOSTR(ch), TOSTR(rch), -1, -1, flags}

static struct {
	int (*stfn) (const char *, struct stat *);
	int (*chfn) (const char *, uid_t, gid_t);
	int (*rchfn) (const char *, uid_t, gid_t);
	const char *stname, *chname, *rchname;
	uid_t uid;
	gid_t gid;
	int flags;
} funcs[] = {
	INIT(stat, chown, lchown, FTW_CHDIR | FTW_PHYS),
	INIT(stat, chown, chown, FTW_CHDIR),
	INIT(NULL, lchown, NULL, 0),
	INIT(lstat, lchown, lchown, FTW_CHDIR | FTW_PHYS)
}, call = INIT(NULL, chown, NULL, 0);

#undef INIT
#undef TOSTR

#endif
