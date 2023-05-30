CC=gcc
CFLAGS=-pthread -lm -lrt
SOURCES = $(wildcard *.c) $(wildcard */*.c)

all:
	$(CC) -o main $(SOURCES) $(CFLAGS)

run: all
	./main
