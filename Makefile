CFLAGS = -Wall -Wpedantic -Wextra -g
LFLAGS = -lwiringPi
CC = g++ #g++ is used because of binary constants (0b110011 for example)

all: matrix test heart heartbeat

%: %.c
	$(CC)  $(CFLAGS) $^ -o $@ $(LFLAGS)