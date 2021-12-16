CC = gcc
CFLAGS = -O2 -Wall
LFLAGS =

all:	lux

lux:	lux.o
	$(CC) $(LFLAGS) -o lux lux.o

lux.o:	lux.c
	$(CC) $(CFLAGS) -c lux.c

clean:
	rm -f *~ *.o lux
