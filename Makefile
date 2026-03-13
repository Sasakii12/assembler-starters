##############################################################################
#
# File:         Makefile
# Date:         Tue 17 Feb 2026  16:58
# Author:       Ken Basye
# Description:  Makefile for Assembler project starter code
#
##############################################################################

# Makefile comments start with #

# First target becomes the default, so just type 'make' to get this 
all: assembler 

# Notes about gcc options:
# -g add degugging information
# -Wall turn on all possible warnings

assembler: assembler.o map.o
	gcc -g -Wall -o assembler assembler.o map.o

maptest: maptest.o map.o
	gcc -g -Wall -o maptest maptest.o map.o

assembler.o: assembler.c map.h
	gcc -c -g -Wall assembler.c

map.o: map.h map.c
	gcc -c -g -Wall map.c

maptest.o: map.h maptest.c
	gcc -c -g -Wall maptest.c

# Use 'make clean' to remove all built files
clean:
	rm -f *.o maptest assembler

