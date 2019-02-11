# Makefile for Maze library
# 	Builds and compiles all these utility functions into a library
#
# 02/25/2018

# object files, and the target library
OBJS = file.o memory.o
LIB = lib.a

# add -DMEMTEST for memory tracking 
FLAGS = #-DMEMTEST

CFLAGS = -Wall -pedantic -std=c11 -ggdb $(FLAGS)
CC = gcc
Make = make

# Build the library by archiving object files
$(LIB): $(OBJS)
	ar cr $(LIB) $(OBJS)

# Dependencies: object files depend on header files
file.o: file.h
memory.o: memory.h

.PHONY: clean sourcelist

# list all the sources and docs in this directory.
sourcelist: Makefile *.md *.c *.h
	@echo $^

# clean up after our compilation
clean:
	rm -f $(LIB) *~ *.o


