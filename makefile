# compiler
CC=g++
FLAGS=-Wall -Wpedantic

# directories
BUILD=build
SRC=src
FRONTEND=frontend
TEST=test

# cpp
all: clean build
	$(CC) $(FLAGS) \
		$(TEST)/$(FRONTEND)/test_lexer.cc \
		$(SRC)/$(FRONTEND)/cish_lexer.cc \
		$(SRC)/$(FRONTEND)/cish_parser.cc \
		$(SRC)/$(FRONTEND)/cish_optimize.cc \
		-o $(BUILD)/test_cish

clean:
	rm -f test_cish

build:
	mkdir -p build
