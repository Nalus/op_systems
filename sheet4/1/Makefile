CFLAGS := -g -Wall -Wextra -pedantic -std=c99 -D_GNU_SOURCE -D_BSD_SOURCE

all: seq_add

seq_add: seq_add.c
	$(CC) $^ -o $@ -lrt

clean:
	$(RM) *.o seq_add
