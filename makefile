SRC=src/
EXE=cl-sv
OBJ=client.o server.o fgeral.o socket.o network.o commands.o
CC =gcc -c -g

all: client server
	rm -f $(OBJ)

client: client.o socket.o network.o commands.o
	gcc client.o socket.o network.o commands.o -o client -g -lm

server: server.o socket.o network.o commands.o
	gcc server.o socket.o network.o commands.o -o server -g -lm


socket.o: $(SRC)socket.c $(SRC)socket.h 
	$(CC) $(SRC)socket.c 

fgeral.o: $(SRC)fgeral.c 
	$(CC) $(SRC)fgeral.c

commands.o: $(SRC)commands.c $(SRC)network.h $(SRC)define.h $(SRC)commands.h
	$(CC) $(SRC)commands.c 

network.o: $(SRC)network.c $(SRC)network.h
	$(CC) $(SRC)network.c

client.o: $(SRC)client.c $(SRC)define.h
	$(CC) $(SRC)client.c 

server.o: $(SRC)server.c $(SRC)define.h
	$(CC) $(SRC)server.c

purge:
	rm  -f $(OBJ) client server