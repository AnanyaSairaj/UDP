$(CC) = gcc
objects = *.o
CFLAGS = -g -Wall
client:
	$(CC) $(CFLAGS) -o udp-client.o 
client.o:
	$(CC) $(CFLAGS) -c udp-client.c	
	
clean:
	rm -f client *.o *.txt *.log

