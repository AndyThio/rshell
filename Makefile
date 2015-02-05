#variables
VPATH = src
CFLAGS = -g -Wall -Werror -ansi -pedantic --std=c++11
FILER = $(addprefix src/, rshell.cpp)
FILELS = $(addprefix src/, ls.cpp)
#target
bin/rshell ls : $(FILER) | bin
	g++ $(CFLAGS) $(FILER) -o $@
	g++ $(CFLAGS) $(FILELS) -o bin/ls

bin:
	mkdir bin

clean:
	rm -rf bin
