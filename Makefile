# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c11

# Typing 'make' or 'make all' will build both the server and client
all: server client

# Rule to build the server
server: server.c
	$(CC) $(CFLAGS) server.c -o server

# Rule to build the client
client: client.c
	$(CC) $(CFLAGS) client.c -o client

# Rule to delete compiled files (cleanup)
clean:
	rm -f server client
