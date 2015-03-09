#variables
VPATH = src
CFLAGS = -g -Wall  -ansi -pedantic --std=c++11
FILER = $(addprefix src/, rshell.cpp)
FILELS = $(addprefix src/, ls.cpp)
#target
bin/ls: bin/rshell | bin
	g++ $(CFLAGS) $(FILELS) -o $@

bin/rshell: $(FILER) | bin
	g++ $(CFLAGS) $(FILER) -o $@

bin:
	mkdir bin

clean:
	rm -rf bin
