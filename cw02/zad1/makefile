CC = gcc
FLAGS = -Wall
LIB_NAME = liblinerem
SOURCE_NAME = liblinerem

clean:
	rm -f *.o

static:
	$(CC) $(FLAGS) -c $(SOURCE_NAME).c -o $(LIB_NAME).o
	ar rcs $(LIB_NAME).a $(LIB_NAME).o
	make clean

run:
	make static
	touch target.txt
	rm target.txt
	touch target.txt

	$(CC) -c main.c
	$(CC) main.o $(LIB_NAME).a -o main

	#gcc main.c -o main
	./main ../source.txt ./target.txt