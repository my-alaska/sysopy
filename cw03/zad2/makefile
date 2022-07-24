run:
	mkdir results
	gcc main.c -o main -lm
	./main 0.0001 2
	make test
	make clean

clean:
	rm -rf results
	rm main

test:
	touch raport.txt
	rm raport.txt
	gcc main.c -o main -lm -D T
	./main 0.000000001 4 >> raport.txt
	./main 0.000000001 8 >> raport.txt
	./main 0.000000001 16 >> raport.txt
	./main 0.000000001 32 >> raport.txt
	./main 0.00000001 4 >> raport.txt
	./main 0.0000001 4 >> raport.txt
	./main 0.000001 4 >> raport.txt
	./main 0.00001 4 >> raport.txt
