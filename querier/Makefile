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

querier.o: querier.c

.PHONY: clean test
test: querier
	chmod a+x testing.sh
	./testing.sh &> testing.out
clean:
	rm -rf *.dSYM
	rm -f *~ *.o
	rm -f $(PROG)
	rm -f core