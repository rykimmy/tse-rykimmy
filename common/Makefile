# Makefile for 'common' module

# Ryan Kim
# CS50, 22S

OBJS = pagedir.o
LIB = ../common/common.a

FLAGS = 
CFLAGS = -Wall -pedantic -std=c11 -ggdb $(FLAGS)
CC = gcc
MAKE = make

$(LIB): $(OBJS)
	ar cr $(LIB) $(OBJS)

pagedir.o: pagedir.h

.PHONY: clean
clean:
	rm -rf *.dSYM
	rm -f $(LIB) *~ *.o
	rm -f core