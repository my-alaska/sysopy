compile: tasks.txt main.c
	gcc -o main main.c
	./main tasks.txt
	rm main

valgrind_test: tasks.txt main.c
	gcc -o main -ggdb3 main.c
	valgrind --leak-check=full --show-leak-kinds=all ./main tasks.txt
	rm main