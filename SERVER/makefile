   
$(CC) = gcc
objects = *.o
server: 
	$(CC) -Wall -Werror udp-server.c -o server
	
client: 
	$(CC) -Wall -Werror  udp-client.c -o client	
	
clean:
	rm -f client $(objects) *.txt *.log	
	
clean:
	rm -f server $(objects) *.txt *.log
