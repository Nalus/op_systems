CFLAGS := -g -Wall -Wextra -pedantic -std=c99 -D_GNU_SOURCE -D_BSD_SOURCE

all: dp_add

dp_add: dp_add.c
	$(CC) $(CFLAGS) $^ -o $@ -lrt

clean:
	$(RM) *.o dp_add
