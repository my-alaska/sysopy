CC = gcc
FLAGS = -Wall
LIB_NAME = liblinerem
SOURCE_NAME = liblinerem

run:
	make static

	$(CC) -c main.c
	$(CC) main.o $(LIB_NAME).a -o main

	./main "u" ../source.txt

clean:
	rm -f *.o

static:
	$(CC) $(FLAGS) -c $(SOURCE_NAME).c -o $(LIB_NAME).o
	ar rcs $(LIB_NAME).a $(LIB_NAME).o
	make clean

