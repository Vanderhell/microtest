CC      ?= gcc
CFLAGS  := -std=c99 -Wall -Wextra -Wpedantic -Werror -I../include

.PHONY: all clean

all: test_all
	@./test_all

test_all: test_all.c ../include/mtest.h
	$(CC) $(CFLAGS) test_all.c -lm -o $@

clean:
	rm -f test_all
