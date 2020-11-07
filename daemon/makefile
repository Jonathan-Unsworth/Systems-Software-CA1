CC = gcc
OBJ = daemon.o client.o server.o daemonize.o
HDR = client.h server.h daemonize.h 

myDaemon: $(OBJ)
	$(CC) -o myDaemon $(OBJ)

client.o: client.c 
	$(CC) -c client.c 

server.o: server.c 
	$(CC) -c server.c 

daemonize.o: daemonize.c $(HDR)
	$(CC) -c daemonize.c 

clean: 
	rm myDaemon $(OBJ)