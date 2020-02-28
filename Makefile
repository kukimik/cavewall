# Makefile for the CAVE WALL editor

CFLAGS = -O3 -Wall

all: cave

cave: -lcurses menu.o block.o conffile.o color.o cursor.o status.o import.o export.o banner.o

clean:
	rm -f cave core *.o
test: clean all
	./cave
count:
	cat *.c *.h Makefile | wc -l
