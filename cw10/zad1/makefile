compile_server: server.c
	gcc server.c -o server -lpthread

compile_client: client.c
	gcc client.c -o client

vtest: server.c
	gcc -o server -ggdb3 server.c -lpthread
	valgrind -s --leak-check=full --show-leak-kinds=all ./server 1400 /tmp/foo
	rm -f server
	rm -f /tmp/asdfgh

vtest_c: client.c
	gcc -o client -ggdb3 client.c
	valgrind -s --leak-check=full --show-leak-kinds=all ./client pigorus local /tmp/foo
	#valgrind -s --leak-check=full --show-leak-kinds=all ./client amogus net 127.0.0.1:1400
	rm -f client


run_server: server.c
	make compile_server
	./server 1400 /tmp/foo
	rm -f server

run_client: client.c
	make compile_client
	./client amogus net 127.0.0.1:1400
	#./client amogus local /tmp/foo
	rm -f client

clean:
	rm -f client