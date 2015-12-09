# makefile for OS Exercises: homework 5
#
#  author:  Dr Hakam W. Alomari - alomarhw@miamioh.edu
#  date:    Nov 2015
#

CC=gcc
CFLAGS= -Wall 
PROGNAME=hostd
SOURCE=$(PROGNAME).c pcb.c
INCLUDE=$(PROGNAME).h pcb.h

$(PROGNAME): $(SOURCE) $(INCLUDE) makefile process
	$(CC) $(CFLAGS) $(SOURCE) -o $@

debug: $(SOURCE) $(INCLUDE) makefile process
	$(CC) $(CFLAGS) -g -D DEBUG $(SOURCE) -o $(PROGNAME)

process: sigtrap.c
	$(CC) $(CFLAGS) $+ -o $@
