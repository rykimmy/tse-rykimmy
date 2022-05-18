# Makefile for 'querier' module
#
# Ryan Kim
# CS50, 22S

PROG = querier
OBJS = querier.o
LIBS = ../common/common.a ../libcs50/libcs50.a

FLAGS = # -DMEMTEST # -DNOSLEEP

CFLAGS = -Wall -pedantic -std=c11 -ggdb $(FLAGS)
CC = gcc
MAKE = make

$(PROG) : $(OBJS) $(LIBS)
	$(CC) $(CFLAGS) $^ -o $@

# querier.o: ../common/pagedir.h ../common/index.h ../common/word.h ../libcs50/file.h
querier.o: querier.c

.PHONY: clean

clean:
	rm -rf *.dSYM
	rm -f *~ *.o
	rm -f $(PROG)
	rm -f core