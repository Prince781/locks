SOURCES=$(wildcard *.c)
CFLAGS=-I. -ggdb3 -O0 -pthread -std=gnu11 -Wall -Wpedantic -Werror

bakery-test: bakery-test.c locks/bakery.h

lamport-test: lamport-test.c locks/lamport.h

spinlock-test: spinlock-test.c locks/spin.h

clean:
	$(RM) bakery-test lamport-test spinlock-test
