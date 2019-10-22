#define more variables so it is easier to make changes
CC=gcc
CFLAGS= -Wall -pthread
TARGETS=reader_writer

all: $(TARGETS)

reader_writer: reader_writer.c
	$(CC) $(CFLAGS) -o $@ $@.c

clean:
	rm -rf *.o *~ $(TARGETS) 
