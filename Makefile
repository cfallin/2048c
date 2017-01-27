CFLAGS = -O3 -Wall
LIBS = -lncurses

.PHONY: all
all: 2048c

2048c: 2048c.c 2048c.h
	$(CC) $(CFLAGS) -o 2048c 2048c.c $(LIBS)

.PHONY: clean
clean:
	rm -f 2048c
