CFLAGS= -O0 -g3
CFLAGS+=-I../../include
CFLAGS+= -I../../deps/libvsync/include

build: example example_mt libmalloc_logger.so

clean:
	rm -rf example example_mt libmalloc_logger.so *.o

libmalloc_logger.so: logger.c
	$(CC) $(CFLAGS) -shared -fPIC -o $@ logger.c

example: malloc_example.c
	$(CC) $(CFLAGS) -o $@ malloc_example.c -lpthread

example_mt: malloc_example_mt.c
	$(CC) $(CFLAGS) -o $@ malloc_example_mt.c -lpthread
