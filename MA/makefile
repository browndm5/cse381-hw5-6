# makefile for OS Exercises: Memory Allocation
#
#

CC=gcc
CFLAGS= -Wall 
PROGNAME=hostd
SOURCE=$(PROGNAME).c pcb.c mab.c
INCLUDE=$(PROGNAME).h pcb.h mab.h

$(PROGNAME): $(SOURCE) $(INCLUDE) makefile process
	$(CC) $(CFLAGS) $(SOURCE) -o $@

debug: $(SOURCE) $(INCLUDE) makefile process
	$(CC) $(CFLAGS) -g -D DEBUG $(SOURCE) -o $(PROGNAME)

process: sigtrap.c
	$(CC) $(CFLAGS) $+ -o $@
