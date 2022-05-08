# Makefile for 'indexer' module

# Ryan Kim
# CS50, 22S

PROG = indexer
OBJS = index.o indexer.o
LIBS = ../common/common.a ../libcs50/libcs50.a

FLAGS = 
CFLAGS = -Wall -pedantic -std=c11 -ggdb $(FLAGS)
CC = gcc
MAKE = make

$(PROG): $(OBJS) $(LIBS)
	$(CC) $(CFLAGS) $^ -O $@

index.o: ../libcs50/hashtable.h ../libcs50/counters.h
indexer.o: ../common/pagedir.h ../common/index.h ../common/word.h

.PHONY: clean
clean:
	rm -rf *.dSYM
	rm -f *~ *.o
	rm -f $(PROG)
	rm -f core