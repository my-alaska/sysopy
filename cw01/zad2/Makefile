CC = gcc
FLAGS = -Wall
LIB_NAME = library
SOURCE_NAME = library


static:
	$(CC) $(FLAGS) -c $(SOURCE_NAME).c -o $(LIB_NAME).o
	ar rcs $(LIB_NAME).a $(LIB_NAME).o
	make clean

shared:
	$(CC) -fPIC $(FLAGS) -c $(SOURCE_NAME).c -o $(LIB_NAME).o
	$(CC) -fPIC $(FLAGS) -shared -o $(LIB_NAME).so $(LIB_NAME).o
	make clean

clean:
	rm -f *.o

clean_all:
	rm main tmp.txt $(LIB_NAME).a $(LIB_NAME).so

test: static
	rm -rf zad.txt
	rm -rf raport2.txt

	#rm tmp*
	$(CC) -c main.c
	$(CC) main.o $(LIB_NAME).a -o main

	@echo "test"
	./main create_table 1 wc_files ../small_file.txt ../small_file2.txt remove_block 0

	@echo "small files (few)" >> raport2.txt
	./main create_table 1 wc_files ../small_file.txt ../small_file2.txt remove_block 0 >> raport2.txt

	@echo "small files (many)" >> raport2.txt
	./main create_table 1 wc_files ../small_file.txt ../small_file2.txt ../small_file3.txt ../small_file4.txt ../small_file5.txt ../small_file6.txt ../small_file7.txt ../small_file8.txt ../small_file9.txt ../small_file10.txt remove_block 0 >> raport2.txt

	@echo "medium files (few)" >> raport2.txt
	./main create_table 1 wc_files ../medium_file.txt ../medium_file2.txt remove_block 0 >> raport2.txt

	@echo "medium files (many)" >> raport2.txt
	./main create_table 1 wc_files ../medium_file.txt ../medium_file2.txt ../medium_file3.txt ../medium_file4.txt ../medium_file5.txt ../medium_file6.txt ../medium_file7.txt ../medium_file8.txt ../medium_file9.txt ../medium_file10.txt remove_block 0 >> raport2.txt

	@echo "large files (few)" >> raport2.txt
	./main create_table 1 wc_files ../large_file.txt ../large_file2.txt remove_block 0 >> raport2.txt

	@echo "large files (many)" >> raport2.txt
	./main create_table 1 wc_files ../large_file.txt ../large_file2.txt ../large_file3.txt ../large_file4.txt ../large_file5.txt remove_block 0 >> raport2.txt

	@echo "A few additions and removals" >> raport2.txt
	./main create_table 2 wc_files ../large_file.txt ../medium_file.txt ../small_file.txt wc_files ../large_file.txt ../medium_file.txt ../small_file.txt remove_block 0 remove_block 1 wc_files ../large_file.txt ../medium_file.txt ../small_file.txt wc_files ../large_file.txt ../medium_file.txt ../small_file.txt remove_block 0 remove_block 1 wc_files ../large_file.txt ../medium_file.txt ../small_file.txt wc_files ../large_file.txt ../medium_file.txt ../small_file.txt remove_block 0 remove_block 1 >> raport2.txt

