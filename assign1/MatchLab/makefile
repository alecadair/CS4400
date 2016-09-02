CC = gcc
FLAGS = -std=c11 -g -Wall 

all: matchlab

matchlab: matchlab.o
	$(CC) $(FLAGS) matchlab.o -o matchlab
	
matchlab.o: matchlab.c
	$(CC) $(FLAGS) -c matchlab.c
	
clean:
	rm *.o matchlab