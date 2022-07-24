compile:
	gcc -o main -Wall main.c
	gcc -o program2 program2.c

runfork:
	./main ignore fork
	./main handler fork
	./main mask fork
	./main pending fork

runexec:
	./main ignore exec
	./main mask exec
	./main pending exec

clean:
	rm -rf main program2

run:
	make compile
	make runfork
	make runexec
	make clean