CC = gcc
CFLAGS = -O0 -g3 -Wall -Wstrict-prototypes -Wmissing-prototypes -Wshadow -Wconversion
LDFLAGS = -lncurses

objects = life.o io.o preset.o

gameoflife: $(objects) main.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(objects): %.o: %.c %.h

main.o: main.c

clean:
	rm $(wildcard *.o) gameoflife

install:
	install gameoflife /usr/bin/  
