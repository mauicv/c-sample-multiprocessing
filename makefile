# build an executable named myprog from myprog.c
all: main.c
	gcc -g -Wall -o main main.c util.c

clean:
	$(RM) main

run:
	echo "running:"
	./main
