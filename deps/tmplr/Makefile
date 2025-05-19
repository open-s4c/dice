.POSIX:

CFLAGS=-Wall -Werror -O3

all: tmplr
clean:
	rm -rf tmplr

tmplr: tmplr.c
	$(CC) $(CFLAGS) -o $@ $<
