TARGETS=all check clean clobber distclean install uninstall
TARGET=all

PREFIX=${DESTDIR}${HOME}
PLUGDIR=${PREFIX}/.purple/plugins
SUBDIRS=

INSTALL=install

.PHONY: ${TARGETS} ${SUBDIRS}

CC	=ccache gcc -std=gnu99 -pipe -shared
CFLAGS	=-Os -Wall -Werror -pedantic -g -fPIC
CFLAGS	+=`pkg-config pidgin --cflags`
CFLAGS	+=`pkg-config gtk+ --cflags`
CFLAGS	+=`pkg-config gdk --cflags`
LDFLAGS	=-g
LDFLAGS	=
LDLIBS	=`pkg-config pidgin --libs`
LDLIBS	+=`pkg-config gtk+ --libs`
LDLIBS	+=`pkg-config gdk --libs`

all::	silence.so

silence.so:: silence.c
	${CC} ${CFLAGS} ${LDFLAGS} -o $@ silence.c ${LDLIBS}

${TARGETS}::

clobber distclean:: clean

check::	silence.so

install:: silence.so
	${INSTALL} -D silence.so ${PLUGDIR}/silence.so

uninstall::
	${RM} ${PLUGDIR}/silence.so

ifneq	(,${SUBDIRS})
${TARGETS}::
	${MAKE} TARGET=$@ ${SUBDIRS}
${SUBDIRS}::
	${MAKE} -C $@ ${TARGET}
endif
