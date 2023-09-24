CC = gcc
CFLAGS = -O0 -g3 -Wall -Wstrict-prototypes -Wmissing-prototypes -Wshadow -Wconversion
LDFLAGS = -lncurses

default:
	$(CC) $(CFLAGS) -o gameoflife life.c io.c main.c $(LDFLAGS)
