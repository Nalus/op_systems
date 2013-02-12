

CFLAGS := -Wall -Wextra -pedantic -std=c99 -D_GNU_SOURCE -D_BSD_SOURCE

all: fork-thread

fork-thread: fork-thread.c
	$(CC) $(CFLAGS) $^ -o $@ -lrt -pthread

clean:
	$(RM) *.o fork-thread
