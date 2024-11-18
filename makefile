CC=gcc

all:
	mkdir -p build
	$(CC) -Wall -Wpedantic src/cish.c -o build/cish

run: all
	./build/cish

valgrind: all
	valgrind -s --leak-check=full --show-leak-kinds=all ./build/cish
