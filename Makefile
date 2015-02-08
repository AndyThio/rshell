#variables
VPATH = src
CFLAGS = -g -Wall -Werror -ansi -pedantic --std=c++11
FILER = $(addprefix src/, rshell.cpp)
FILELS = $(addprefix src/, ls.cpp)
#target
bin/rshell: bin/ls | bin
	g++ $(CFLAGS) $(FILER) -o $@

bin/ls: $(FILELS) | bin
	g++ $(CFLAGS) $(FILELS) -o $@

bin:
	mkdir bin

clean:
	rm -rf bin
