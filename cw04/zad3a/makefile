compile: catcher.c sender.c
	gcc -o catcher catcher.c
	gcc -o sender sender.c

clean: catcher sender
	rm catcher sender

# first catcher argument and last sender argument should both be the same member of {KILL, SIGQUEUE, SIGRT} set
catch: catcher
	./catcher SIGRT

send: sender
	./sender 4924 200 SIGRT # use pid printed by catcher as the first argument passed to sender