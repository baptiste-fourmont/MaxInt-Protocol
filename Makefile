PROG = gcc -Wall -g -I include/

all: client1 client2 server

utils.o: src/utils.c
	$(PROG) -c src/utils.c -lsocket

client1: utils.o client1.o
	$(PROG) -o client1 utils.o client1.o

client1.o: src/client1.c 
	$(PROG) -c src/client1.c
    
client2: utils.o client2.o
	$(PROG) -o client2 utils.o client2.o

client2.o: src/client2.c 
	$(PROG) -c src/client2.c

server: utils.o server.o
	$(PROG) -pthread -o server utils.o server.o

server.o: src/server.c 
	$(PROG) -c src/server.c

clean: 
	rm -f *.o client1 client2 server
