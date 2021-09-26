CC =gcc
CFLAGS= -Wall -Werror
client: $(CLIENT)/udp-client.c 
	$(CC) $(CFLAGS) -o udp-client.o -c $(CLIENT)/udp-client.c
	

