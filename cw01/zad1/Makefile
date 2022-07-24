CC = gcc
FLAGS = -Wall
LIB_NAME = library
SOURCE_NAME = library


static:
	gcc -Wall -c library.c -o library.o
	ar rcs library.a library.o
	make clean

shared:
	gcc -fPIC -Wall -c library.c -o library.o
	gcc -fPIC -Wall -shared -o library.so library.o
	make clean

clean:
	rm -f *.o

clean_all:
	rm main tmp.txt library.a library.so