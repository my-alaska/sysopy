compile:
	gcc -o main main.c
	./main nadawca
	./main data
	./main sysopy trudne som
	rm main

valgrind_test:
	gcc -o main -ggdb3 main.c
	valgrind --leak-check=full --show-leak-kinds=all ./main nadawca
	valgrind --leak-check=full --show-leak-kinds=all ./main data
	valgrind --leak-check=full --show-leak-kinds=all ./main sysopy trudne som