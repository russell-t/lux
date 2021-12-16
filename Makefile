CC = gcc
CFLAGS = -O2 -Wall
LFLAGS =

all:	main

main:	main.o i2c.o
	$(CC) $(LFLAGS) main.o i2c.o -o main

main.o:	main.c i2c.h
	$(CC) $(CFLAGS) -c main.c

i2c.o:	i2c.c i2c.h
	$(CC) $(CFLAGS) -c i2c.c

clean:
	rm -f *~ *.o main
