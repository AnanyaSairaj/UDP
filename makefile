$(CC) = gcc
objects = *.o
all:
	$(CC) -Wall -Werror  udp-server.c -o server 
	$(CC) -Wall -Werror  udp-client.c -o client	
	
clean:  server client
	rm -f client $(objects) *.txt *.log
	rm -f server $(objects) *.txt *.log
