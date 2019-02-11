# Makefile for top-level Maze Challenge
#
# 03/01/2018

MAKE = make
.PHONY: all valgrind clean

################### default: make all libs and programs #########
all:
	$(MAKE) -C lib
	$(MAKE) -C src

################## valgrind all programs ##################
valgrind: 
	$(MAKE) -C amstartup valgrind

################## clean ########################
clean: 
	rm -f *~
	$(MAKE) -C lib clean
	$(MAKE) -C src clean

