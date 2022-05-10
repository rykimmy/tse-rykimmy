# Makefile for 'indexer' module

# Ryan Kim
# CS50, 22S

PROG = indexer
PROG2 = indextest
# removed ../common/index.o from below
OBJS = indexer.o
OBJS2 = indextest.o
LIBS = ../common/common.a ../libcs50/libcs50.a

FLAGS = 
CFLAGS = -Wall -pedantic -std=c11 -ggdb $(FLAGS)
CC = gcc
MAKE = make

$(PROG): $(OBJS) $(LIBS)
	$(CC) $(CFLAGS) $^ -o $@

$(PROG2): $(OBJS2) $(LIBS)
	$(CC) $(CFLAGS) $^ -o $@

indexer.o: indexer.c

indextest.o: indextest.c

.PHONY: all test clean

all: indexer indextest

test: indexer indextest
	./testing.sh &> testing.out

clean:
	rm -rf *.dSYM
	rm -f *~ *.o
	rm -f $(PROG)
	rm -f $(PROG2)
	rm -f core