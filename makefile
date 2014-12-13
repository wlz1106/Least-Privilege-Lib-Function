LIBS=-lpthread -lm
CPPFLAGS=-std=c++11
CFLAGS=-std=c99
CC=gcc
CPP=g++

TEST_SRC=$(wildcard test/test_*.c)
TEST_EXE=$(TEST_SRC:%.c=%)

all:tracelib $(TEST_EXE)

.SECONDEXPANSION:
$(TEST_EXE):$$@.c
	$(CC) -o $@ $< $(CFLAGS) $(LIBS)
tracelib:tracelib.cpp tracelib.hpp
	$(CPP) -o $@ $< $(CPPFLAGS)
clean:
	rm -f tracelib *.txt $(TEST_EXE) 
ifeq ($(CLEAN_RESULT),1)	
	./clean_test_result.sh
endif

