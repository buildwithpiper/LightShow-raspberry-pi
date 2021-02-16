CFLAGS = -Wall -Wpedantic -Wextra -g -Wno-missing-field-initializers
LFLAGS = -lwiringPi

all: matrix test

matrix: matrix.c
	$(CC)  $(CFLAGS) $^ -o $@ $(LFLAGS)

test: test.c
	$(CC)  $(CFLAGS) $^ -o $@ $(LFLAGS)