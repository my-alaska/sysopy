F1 = brain_398.ascii.pgm
F2 = apollonian_gasket.ascii.pgm

vtest: main.c
	gcc -o main -ggdb3 main.c -lpthread
	valgrind --leak-check=full --show-leak-kinds=all ./main 1 numbers $(F1) output_file.ascii.pgm
	make clean

compile: main.c
	gcc -Wall main.c -o main -pthread

run: main.c
	make compile
	@echo $(F1) > Times.txt
	@echo "\nblock" >> Times.txt
	./main 1 block $(F1) output_file.ascii.pgm >> Times.txt
	./main 2 block $(F1) output_file.ascii.pgm >> Times.txt
	./main 4 block $(F1) output_file.ascii.pgm >> Times.txt
	./main 8 block $(F1) output_file.ascii.pgm >> Times.txt
	./main 16 block $(F1) output_file.ascii.pgm >> Times.txt

	@echo "\nnumbers" >> Times.txt
	./main 1 numbers $(F1) output_file.ascii.pgm >> Times.txt
	./main 2 numbers $(F1) output_file.ascii.pgm >> Times.txt
	./main 4 numbers $(F1) output_file.ascii.pgm >> Times.txt
	./main 8 numbers $(F1) output_file.ascii.pgm >> Times.txt
	./main 16 numbers $(F1) output_file.ascii.pgm >> Times.txt

	@echo "\n\n\n" >> Times.txt

	@echo $(F2) >> Times.txt
	@echo "\nblock" >> Times.txt
	./main 1 block $(F2) output_file.ascii.pgm >> Times.txt
	./main 2 block $(F2) output_file.ascii.pgm >> Times.txt
	./main 4 block $(F2) output_file.ascii.pgm >> Times.txt
	./main 8 block $(F2) output_file.ascii.pgm >> Times.txt
	./main 16 block $(F2) output_file.ascii.pgm >> Times.txt

	@echo "\nnumbers" >> Times.txt
	./main 1 numbers $(F2) output_file.ascii.pgm >> Times.txt
	./main 2 numbers $(F2) output_file.ascii.pgm >> Times.txt
	./main 4 numbers $(F2) output_file.ascii.pgm >> Times.txt
	./main 8 numbers $(F2) output_file.ascii.pgm >> Times.txt
	./main 16 numbers $(F2) output_file.ascii.pgm >> Times.txt

	make clean

clean:
	rm -f main