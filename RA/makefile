# makefile for OS Exercises: exercise Resource Allocation & project final
#
#  author:  Dr Hakam W. Alomari 
#  date:    Nov 2015
#

CC=gcc
CFLAGS= -Wall 
PROGNAME=hostd
SOURCE=$(PROGNAME).c pcb.c mab.c rsrc.c
INCLUDE=$(PROGNAME).h pcb.h mab.h rsrc.h

$(PROGNAME): $(SOURCE) $(INCLUDE) makefile process
	$(CC) $(CFLAGS) $(SOURCE) -o $@

debug: $(SOURCE) $(INCLUDE) makefile process
	$(CC) $(CFLAGS) -g -D DEBUG $(SOURCE) -o $(PROGNAME)

process: sigtrap.c
	$(CC) $(CFLAGS) $+ -o $@
