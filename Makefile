CC = gcc

all: server client

serverdaniel: server.c
	$(CC)  $^ -o $@ 

clientdaniel: client.c
	$(CC)  $^ -o $@

clean:
	rm -f server client

.PHONY: all clean


