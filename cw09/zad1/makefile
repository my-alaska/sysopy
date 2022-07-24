vtest: main.c
	gcc -o main -ggdb3 main.c -lpthread
	valgrind -s --leak-check=full --show-leak-kinds=all ./main
	make clean

compile: main.c
	gcc -Wall main.c -o main -pthread

run: main.c
	make compile
	./main
	make clean

clean:
	rm -f main
