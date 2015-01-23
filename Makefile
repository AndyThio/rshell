main: rshell
	rm rshell
	g++ -std=c++11 rshell.cpp -o rshell

gdb: rshell
	rm rshell
	g++ -std=c++11 -g rshell.cpp -o rshell
