CC=gcc

all:
	mkdir -p build
	$(CC) -Wall -Wpedantic src/cish.c -o build/cish
