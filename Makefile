#variables
VPATH = src
CFLAGS = -std=c++11 -Wall -Werror -ansi -pedantic
FILES = $(addprefix src/, rshell.cpp)
#target
bin/rshell: rshell.cpp | bin
	g++ $(CFLAGS) $(FILES) -o $@

bin:
	mkdir bin

clean:
	rm -rf bin
