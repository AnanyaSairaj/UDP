$(CC) = gcc
objects = *.o
CFLAGS = -g -Wall
client: client.o
	$(CC) $(CFLAGS) -o udp-client.o 
client.o: client.c
	$(CC) $(CFLAGS) -c udp-client.c	
	
clean:
	rm -f client $(objects)*.txt *.log

