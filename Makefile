CC = gcc
LDFLAGS = -lncurses

default:
	$(CC) -o gameoflife life.c io.c main.c $(LDFLAGS)

debug: 
	$(CC) -o test life.c io.c test.c $(LDFLAGS)
