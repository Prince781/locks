SOURCES=$(wildcard *.c)
CFLAGS=-I. -ggdb3 -O0 -pthread -std=gnu11 -Wall -Wpedantic -Werror

all: bakery-test lamport-test spinlock-test ticket-test mcs-test

bakery-test: bakery-test.c locks/bakery.h

lamport-test: lamport-test.c locks/lamport.h

spinlock-test: spinlock-test.c locks/spin.h

ticket-test: ticket-test.c locks/ticket.h

mcs-test: mcs-test.c locks/mcs.h

clean:
	$(RM) bakery-test lamport-test spinlock-test ticket-test mcs-test
