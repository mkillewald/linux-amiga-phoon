# Makefile for phoon

BINDIR =	/usr/local/bin
MANDIR =	/usr/local/man/man1

DEFINES =	-DOS_BSD
#DEFINES =	-DOS_SYSV

CC =		cc
CFLAGS =	-O $(DEFINES) -ansi -pedantic -U__STRICT_ANSI__ -Wall -Wpointer-arith -Wshadow -Wcast-qual -Wcast-align -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -Wredundant-decls -Wno-long-long
LDFLAGS =	-s


all:		phoon

phoon:		phoon.o date_parse.o date_calc.o astro.o
	$(CC) $(LDFLAGS) -o ./linux/phoon phoon.o date_parse.o date_calc.o astro.o -lm

.c.o:
	$(CC) $(CFLAGS) -c $<

date_parse.o:	date_parse.h
date_calc.o:	date_calc.h
astro.o:	astro.h

install:	all
	rm -f $(BINDIR)/phoon
	cp phoon $(BINDIR)
	rm -f $(MANDIR)/phoon.1
	cp phoon.1 $(MANDIR)

clean:
	rm -f ./linux/phoon *.o a.out core
