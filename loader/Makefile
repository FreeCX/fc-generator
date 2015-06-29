CC=gcc
SOURCE=loader.c stack.c
CFLAGS=-std=c11 -Wall $(shell sdl2-config --cflags --libs) -lSDL2_image
PROG=loader

all:
	$(CC) $(SOURCE) -o $(PROG) $(CFLAGS)
clean:
	$(RM) $(PROG)
