CFLAGS := -g -Wall -Wextra -pedantic -std=c99 -D_GNU_SOURCE -D_BSD_SOURCE

TESTS := malloc-tester-01

all: $(TESTS)

malloc-tester%: malloc-tester%.o
	$(CC) $^ -o $@ -lrt

malloc-tester%.o: malloc-tester.h

test: $(TESTS)
	./malloc-tester-01

clean:
	$(RM) *.o $(TESTS)

