VPATH = src:lib
OBJ_DIR := obj
BIN_DIR := bin
FLAGS := -Wall -Wextra -std=c99


all: client server

debug: FLAGS += -DDEBUG -g
debug: all

client: client.o socket.o connection.o command.o utils.o log.o
		gcc $(FLAGS) obj/log.o obj/command.o obj/client.o obj/socket.o obj/connection.o obj/utils.o -o $(BIN_DIR)/client -lm -lssl -lcrypto

server: server.o socket.o connection.o command.o log.o utils.o
		gcc $(FLAGS) obj/server.o obj/socket.o obj/connection.o obj/command.o obj/log.o obj/utils.o -o $(BIN_DIR)/server -lm -lssl -lcrypto

server.o: server.c
		gcc $(FLAGS) -c src/server.c -o obj/server.o

client.o: client.c | $(OBJ_DIR) $(BIN_DIR)
		gcc $(FLAGS) -c src/client.c -o obj/client.o 

socket.o: socket.h
		gcc $(FLAGS) -c src/socket.c -o obj/socket.o

connection.o: connection.h
		gcc $(FLAGS) -c src/connection.c -o obj/connection.o

command.o: command.h 
		gcc $(FLAGS) -c src/command.c -o obj/command.o  -lm

utils.o: utils.h
		gcc $(FLAGS) -c src/utils.c -o obj/utils.o

log.o: log.h
		gcc $(FLAGS) -c src/log.c -o obj/log.o 

$(OBJ_DIR) $(BIN_DIR):
		mkdir -p $@

clean:
		-rm -f $(OBJ_DIR)/* $(BIN_DIR)/*
