.SUFFIXES: .in

PAGES=	tmplr.1 tmplr.3 tmplr_macros.3

all: ${PAGES}
clean:
	rm -f ${PAGES}
.in:
	../.versionize.sh -r $< > $@
