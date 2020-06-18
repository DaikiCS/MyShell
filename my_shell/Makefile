# build ssh -- try each one, will build latest
ssh: ssh.h ssh.c array.o command.o parse.o builtins.o
	gcc -g -o ssh ssh.c array.o command.o builtins.o parse.o -l readline -D _GNU_SOURCE

array.o: array.h array.c
	gcc -c array.c

command.o: command.h command.c array.o
	gcc -c command.c

parse.o: parse.h parse.c array.o
	gcc -c parse.c

builtins.o: builtins.h builtins.c
	gcc -c builtins.c
