CFLAGS := -g -Wall -Wextra -pedantic -std=c99 -D_GNU_SOURCE -D_BSD_SOURCE -Dmalloc=mymalloc -Dfree=myfree

TESTS := frag 

all: $(TESTS)

frag%: frag%.o mymalloc.o
	$(CC) $^ -o $@ -lrt

frag%.o: mymalloc.h

test: $(TESTS)
	./frag

clean:
	$(RM) *.o $(TESTS)
