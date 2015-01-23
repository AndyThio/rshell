build:
	g++ -std=c++11 -Wall -Werror -ansi -pedantic rshell.cpp -o rshell
main: rshell
	rm rshell
	g++ -std=c++11 -Wall -Werror -ansi -pedantic rshell.cpp -o rshell

gdb: rshell
	rm rshell
	g++ -std=c++11 -g -Wall -Werror -ansi -pedantic rshell.cpp -o rshell
