SOURCES=$(wildcard *.c)
CFLAGS=-I. -ggdb3 -O0 -pthread -std=gnu11 -Wall -Wpedantic -Werror

bakery-test: bakery-test.c locks/bakery.h

clean:
	$(RM) bakery-test
