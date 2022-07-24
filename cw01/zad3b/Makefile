CC = gcc
FLAGS = -Wall
LIB_NAME = library
LIB_PATH = ../zad3
SOURCE_NAME = library
export LD_LIBRARY_PATH = $(shell pwd)


static:
	$(CC) $(FLAGS) -c $(SOURCE_NAME).c -o $(LIB_NAME).o
	ar rcs $(LIB_NAME).a $(LIB_NAME).o
	make clean

shared:
	$(CC) -fPIC $(FLAGS) -c $(SOURCE_NAME).c -o $(LIB_NAME).o
	$(CC) -fPIC $(FLAGS) -shared -o $(LIB_NAME).so $(LIB_NAME).o
	make clean

static_runtest:
	#rm tmp*
	make static
	@echo "STATIC o1" >> raport3b.txt
	$(CC) -O1 -c main.c -o main.o
	$(CC) -O1 main.o $(LIB_NAME).a -o main
	make test
	@echo "STATIC o0" >> raport3b.txt
	$(CC) -O0 -c main.c -o main.o
	$(CC) -O0 main.o $(LIB_NAME).a -o main
	make test
	@echo "STATIC os" >> raport3b.txt
	$(CC) -Os -c main.c -o main.o
	$(CC) -Os main.o $(LIB_NAME).a -o main
	make test

shared_runtest:
	make shared
	@echo "SHARED o1" >> raport3b.txt
	$(CC) -O1 main main.c -L. -lrary
	make test
	@echo "SHARED o0" >> raport3b.txt
	$(CC) -O0 main main.c -L. -lrary
	make test
	@echo "SHARED os" >> raport3b.txt
	$(CC) -Os main main.c -L. -lrary
	make test

dynamic_runtest:
	make shared
	@echo "DYNAMIC o1" >> raport3b.txt
	$(CC) -O1 main main.c -I. -ldl -D DYNAMIC
	make test
	@echo "DYNAMIC o0" >> raport3b.txt
	$(CC) -O0 main main.c -I. -ldl -D DYNAMIC
	make test
	@echo "DYNAMIC os" >> raport3b.txt
	$(CC) -Os main main.c -I. -ldl -D DYNAMIC
	make test

compile_tests:
	rm -rf raport3b.txt
	rm -rf raport3b.txt
	make static_runtest
	make shared_runtest
	make dynamic_runtest

test:
	@echo "test"
	./main create_table 1 wc_files ../small_file.txt ../small_file2.txt remove_block 0

	@echo "small files (few)" >> raport3b.txt
	./main create_table 1 wc_files ../small_file.txt ../small_file2.txt remove_block 0 >> raport3b.txt

	@echo "small files (many)" >> raport3b.txt
	./main create_table 1 wc_files ../small_file.txt ../small_file2.txt ../small_file3.txt ../small_file4.txt ../small_file5.txt ../small_file6.txt ../small_file7.txt ../small_file8.txt ../small_file9.txt ../small_file10.txt remove_block 0 >> raport3b.txt

	@echo "medium files (few)" >> raport3b.txt
	./main create_table 1 wc_files ../medium_file.txt ../medium_file2.txt remove_block 0 >> raport3b.txt

	@echo "medium files (many)" >> raport3b.txt
	./main create_table 1 wc_files ../medium_file.txt ../medium_file2.txt ../medium_file3.txt ../medium_file4.txt ../medium_file5.txt ../medium_file6.txt ../medium_file7.txt ../medium_file8.txt ../medium_file9.txt ../medium_file10.txt remove_block 0 >> raport3b.txt

	@echo "large files (few)" >> raport3b.txt
	./main create_table 1 wc_files ../large_file.txt ../large_file2.txt remove_block 0 >> raport3b.txt

	@echo "large files (many)" >> raport3b.txt
	./main create_table 1 wc_files ../large_file.txt ../large_file2.txt ../large_file3.txt ../large_file4.txt ../large_file5.txt remove_block 0 >> raport3b.txt

	@echo "A few additions and removals" >> raport3b.txt
	./main create_table 2 wc_files ../large_file.txt ../medium_file.txt ../small_file.txt wc_files ../large_file.txt ../medium_file.txt ../small_file.txt remove_block 0 remove_block 1 wc_files ../large_file.txt ../medium_file.txt ../small_file.txt wc_files ../large_file.txt ../medium_file.txt ../small_file.txt remove_block 0 remove_block 1 wc_files ../large_file.txt ../medium_file.txt ../small_file.txt wc_files ../large_file.txt ../medium_file.txt ../small_file.txt remove_block 0 remove_block 1 >> raport3b.txt

clean:
	rm -f *.o

clean_all:
	rm main tmp.txt $(LIB_NAME).a $(LIB_NAME).so




