CFLAGS := -g -Wall -Wextra -pedantic -std=c99 -D_GNU_SOURCE -D_BSD_SOURCE -Dmalloc=mymalloc -Dfree=myfree

TESTS := malloc-tester-01 malloc-tester-02 malloc-tester-03 malloc-tester-04

all: $(TESTS)

malloc-tester%: malloc-tester%.o mymalloc.o
	$(CC) $^ -o $@ -lrt

malloc-tester%.o: malloc-tester.h mymalloc.h

test: $(TESTS)
	./malloc-tester-01
	./malloc-tester-02
	./malloc-tester-03
	./malloc-tester-04

clean:
	$(RM) *.o $(TESTS)
