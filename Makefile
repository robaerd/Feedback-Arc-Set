# @name Makefile
# @author Robert Sowula <e1170475@student.tuwien.ac.at>

CC = gcc
DEFS = -D_BSD_SOURCE -D_SVID_SOURCE -D_POSIX_C_SOURCE=200809L
CFLAGS = -Wall -g -std=c99 -pedantic $(DEFS)
LDFLAGS =  -lrt -lpthread

OBJECTSSUPERVISOR = supervisor.o error.o
OBJECTSGENERATOR =  generator.o topologicalSol.o error.o

.PHONY: all clean
all: supervisor generator

supervisor: $(OBJECTSSUPERVISOR)
	$(CC) -o $@ $^  $(LDFLAGS)

generator: $(OBJECTSGENERATOR)
	$(CC) -o $@ $^  $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

supervisor.o: supervisor.c supervisor.h circularBuffer.h
generator.o: generator.c generator.h circularBuffer.h
topologicalSol.o: topologicalSol.c generator.h circularBuffer.h
error.o: error.c error.h

clean:
	rm -rf *.o supervisor generator
