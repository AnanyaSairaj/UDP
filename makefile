TARGET=client server 
CC=gcc
CFLAGS= -Wall -Wextra -g
normal: $(TARGET)
client: udp-client.c
    $(CC) $(CFLAGS) udp-client.c -o client
server: server.c
    $(CC) $(CFLAGS) udp-server.c -o server
clean:
    $(RM) $(TARGET)
