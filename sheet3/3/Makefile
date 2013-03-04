CFLAGS := -g -Wall -Wextra -pedantic -std=c99 -D_GNU_SOURCE -D_BSD_SOURCE -Dmalloc=mymalloc -Dfree=myfree

TESTS := small

all: $(TESTS)

small: small.o mymalloc.o
	$(CC) $^ -o $@ -lrt

small.o: mymalloc.h

test: $(TESTS)
	./small

clean:
	$(RM) *.o $(TESTS)
