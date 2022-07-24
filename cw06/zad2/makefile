# Open 3 terminals and navigate to the "zad2" directory in all of them
#in the first one type "make compile" and then "./server"
#in the second one type HOME=/tmp and then "./client"
#in the  third one type "./client"

#in clients type LIST and enter to list out commands
#type STOP and enter to terminate the client
#type TOALL message and enter to send message to all clients
#type TOONE i message and enter to send message to user i
#type GETMSG to see if any signals have been received from the server

compile: server.c client.c
	rm -f file.txt
	gcc -o server server.c -lrt
	gcc -o client client.c -lrt

clean:
	rm -f file.txt
	rm -f server
	rm -f client
