# Makefile for 'crawler' module
# 	Builds and compiles all these utility functions
#
# Ryan Kim
# CS50, 22S
#
# Object files
PROG = crawler
OBJS = crawler.o
LIBS = ../common/common.a ../libcs50/libcs50.a

FLAGS = # -DMEMTEST # -DNOSLEEP

CFLAGS = -Wall -pedantic -std=c11 -ggdb $(FLAGS)
CC = gcc
MAKE = make

$(PROG) : $(OBJS) $(LIBS)
	$(CC) $(CFLAGS) $^ -o $@

crawler.o:  ../libcs50/memory.h ../libcs50/bag.h ../libcs50/webpage.h ../libcs50/hashtable.h ../common/pagedir.h

.PHONY: clean test
test: $(PROG)
	./testing.sh &> testing.out

clean:
	rm -rf *.dSYM
	rm -f *~ *.o
	rm -f $(PROG)
	rm -f core
	rm -f pages/*