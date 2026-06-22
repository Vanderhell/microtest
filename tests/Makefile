CC      ?= gcc
MTEST_ENABLE_FLOAT ?= 1
CFLAGS  := -std=c99 -Wall -Wextra -Wpedantic -Werror -I../include -DMTEST_ENABLE_FLOAT=$(MTEST_ENABLE_FLOAT)
LDLIBS  :=
ifeq ($(MTEST_ENABLE_FLOAT),1)
LDLIBS  += -lm
endif

.PHONY: all clean

all: test_all
	@./test_all

test_all: test_all.c ../include/mtest.h
	$(CC) $(CFLAGS) test_all.c $(LDLIBS) -o $@

clean:
	rm -f test_all
