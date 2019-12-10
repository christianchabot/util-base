# Modify the following to your preference.
#PREFIX = 
#BINDIR = ${PREFIX}/bin
#USRBINDIR = ${PREFIX}/usr/bin
#SBINDIR = ${PREFIX}/sbin
#USRSBINDIR = ${PREFIX}/usr/sbin

CFLAGS = -D _XOPEN_SOURCE=700 -s

# Programs that a deficient shell would need should go in /bin also.
# Comment out the programs you do not want or move into different dir.

BIN =\
	cat\
	chgrp\
	chmod\
	chown\
	cp\
	date\
	echo\
	env\
	false\
	kill\
	link\
	mkdir\
	pwd\
	rm\
	rmdir\
	sleep\
	stty\
	true\
	tty\
	unlink

USRBIN =\
	basename\
	cal\
	cmp\
	df\
	dirname\
	du\
	expand\
	head\
	id\
	logname\
	nice\
	nohup\
	time\
	touch\
	uname\
	wc

SBIN = 
USRSBIN = 
# End of modification.

CMDS = ${BIN} ${USRBIN} ${SBIN} ${USRSBIN}
LIBS = lib/io.c lib/err.c lib/convert.c lib/mode.c lib/path.c lib/util.c
HDRS = ch.h io.h mode.h path.h util.h

.SUFFIXES: .c

all: ${CMDS}

# Should create .o files for library and then use ar command to create library.
#${LIBRARY}:
#	${CC} -c ${LIBS}
#	${AR} 

${CMDS}: ${LIBS} ${HDRS} ${CMDS:=.c}
	${CC} $(CFLAGS) -o $@ ${@:=.c} ${LIBS}

clean:
	rm -f ${CMDS}

# Move the programs to the directories.
#install: installbin installusrbin installsbin installusrsbin

installbin: ${BIN}
	# test -d ${BIN} && (test -f ${BIN} || cp ${BIN} ${BINDIR})
	cp ${BIN} ${BINDIR}

installusrbin: ${USRBIN}
	cp ${USRBIN} ${USRBINDIR}

installsbin: ${SBIN}
	cp ${SBIN} ${SBINDIR}

installusrsbin: ${USRSBIN}
	cp ${USRSBIN} ${USRSBINDIR}


# Remove the programs from the directories. (Maybe keep)
uninstall:
