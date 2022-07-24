run_valrind: main.c
	gcc -o main -ggdb3 main.c -lpthread -lrt
	valgrind --leak-check=full --show-leak-kinds=all ./main 6 7

compile: main.c
	gcc -o main main.c -lpthread -lrt

run: main.c
	make compile
	./main 5 5



test1: main.c
	make compile
	./main 7 7

test2: main.c
	make compile
	./main 3 3



clean:
	rm -f main