CFLAGS = -Wall -Wpedantic -Wextra -g
LFLAGS = -lwiringPi
CC = g++ #g++ is used because of support for binary constants (0b110011 for example)

all: matrix test

matrix: matrix.c
	$(CC)  $(CFLAGS) $^ -o $@ $(LFLAGS)

test: test.c
	$(CC)  $(CFLAGS) $^ -o $@ $(LFLAGS)