LIBS=-lpthread -lm
TEST_EXE="test/"
all:tracelib test 

test:test/test_math test/test_pthread test/test_cipher

test/test_math:
	gcc -o $@.exe $@.c -lm
test/test_pthread:
	gcc -o $@.exe $@.c -lpthread
test/test_cipher:
	gcc -o $@.exe $@.c 
tracelib:tracelib.cpp tracelib.hpp
ifeq ($(DEBUG),1)
	g++ -std=c++11 -o tracelib -g tracelib.cpp
else
	g++ -std=c++11 -o tracelib tracelib.cpp
endif
clean:
	rm -f tracelib *.txt test/*.exe

