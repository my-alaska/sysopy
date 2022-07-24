run:
	gcc -o main main.c
	./main siginfo sigchld
	./main siginfo sigint
	./main siginfo sigusr1

	# testing other flags
	./main onstack 0
	./main resethand 0
	rm main