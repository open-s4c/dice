.POSIX:

CFLAGS=		-O3
CFLAGS.tmplr=	-std=c99 -Iinclude -MMD -MP
CFLAGS.tmplr+=	-Wall -Wextra -Wpedantic -Wshadow -Werror
CFLAGS.cov=	-g -O0 --coverage
AR=		ar

PREFIX=		/usr/local
BINDIR=		${PREFIX}/bin
LIBDIR=		${PREFIX}/lib
MANDIR=		${PREFIX}/share/man
INCLUDEDIR=	${PREFIX}/include
SHAREDIR=	${PREFIX}/share/tmplr

all: tmplr libtmplr.a
	@cd man && ${MAKE}

clean:
	rm -rf tmplr libtmplr.a tmplr.o version.h tmplr.1 *.o *.d
	@cd test && ${MAKE} clean
	@cd man && ${MAKE} clean

tmplr: version.h tmplr.c
	${CC} ${CFLAGS.tmplr} ${CFLAGS} -o $@ tmplr.c

tmplr.o: version.h tmplr.c
	${CC} ${CFLAGS.tmplr} ${CFLAGS} -DTMPLR_NO_MAIN -c -o $@ tmplr.c

libtmplr.a: tmplr.o
	${AR} rcs $@ tmplr.o

version.h: version.h.in
	./.versionize.sh -r version.h.in > $@

coverage: clean
	${MAKE} CFLAGS="${CFLAGS.cov}" all

SAN=address
sanitize: clean
	${MAKE} CFLAGS="-fsanitize=${SAN} -O0 -g" all

install: all
	mkdir -p ${DESTDIR}${BINDIR} ${DESTDIR}${LIBDIR}
	mkdir -p ${DESTDIR}${MANDIR}/man1 ${DESTDIR}${MANDIR}/man3
	mkdir -p ${DESTDIR}${INCLUDEDIR}/tmplr
	mkdir -p ${DESTDIR}${SHAREDIR}
	install -m 755 tmplr ${DESTDIR}${BINDIR}/
	install -m 644 libtmplr.a ${DESTDIR}${LIBDIR}/
	install -m 644 man/tmplr.1 ${DESTDIR}${MANDIR}/man1/
	install -m 644 man/tmplr.3 ${DESTDIR}${MANDIR}/man3/
	install -m 644 man/tmplr_macros.3 ${DESTDIR}${MANDIR}/man3/
	install -m 644 include/tmplr.h ${DESTDIR}${INCLUDEDIR}/
	install -m 644 include/tmplr/macros.h ${DESTDIR}${INCLUDEDIR}/tmplr/
	install -m 755 share/tmplr/ensure-cmd.sh ${DESTDIR}${SHAREDIR}/

test: all
	@cd test && ${MAKE}

format:
	@find . -name '*.h' -exec clang-format -i -style=file {} +
	@find . -name '*.c' -exec clang-format -i -style=file {} +

DEPS=	$(shell find . -name '*.d')
DEPS!=	touch version.d && find . -name '*.d'
include ${DEPS}
