# Makefile for 'crawler' module
# 	Builds and compiles all these utility functions

# Ryan Kim
# CS50, 22S

# Object files
PROG = crawler
OBJS = crawler.o
LIBS = ../libcs50/libcs50-given.a ../common/common.a

FLAGS = # -DMEMTEST # -DNOSLEEP

CFLAGS = -Wall -pedantic -std=c11 -ggdb $(FLAGS)
CC = gcc
MAKE = make

$(PROG) : $(OBJS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

crawler.o:  ../libcs50/memory.h ../libcs50/bag.h ../libcs50/webpage.h ../libcs50/hashtable.h ../common/pagedir.h

.PHONY: clean test
test: $(PROG)
	./testing.sh &> testing.out

clean:
	rm -rf *.dSYM
	rm -f *~ *.object
	rm -f $(PROG)
	rm -f core