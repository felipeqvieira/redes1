SRC=src/
EXE=cl-sv
OBJ=client.o server.o fgeral.o raw.o cl-sv.o
CC =gcc -c -g

all: $(OBJ)
	gcc $(OBJ) -o $(EXE) -g -lm
	rm -f $(OBJ)
	
cl-sv.o: $(SRC)cl-sv.c $(SRC)define.h
	$(CC) $(SRC)cl-sv.c 

client.o: $(SRC)client.c $(SRC)define.h  
	$(CC) $(SRC)client.c 

server.o: $(SRC)server.c $(SRC)define.h 
	$(CC) $(SRC)server.c 

raw.o: $(SRC)raw.c $(SRC)define.h 
	$(CC) $(SRC)raw.c 

fgeral.o: $(SRC)fgeral.c $(SRC)define.h
	$(CC) $(SRC)fgeral.c 

purge:
	rm  -f $(OBJ) $(EXE)
debug:
	valgrind --verbose --leak-check=full --track-origins=yes ./cl-sv -t c -i eth0