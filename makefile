TARGET=client server 
CC=gcc
CFLAGS= -Wall -Wextra -g
normal: $(TARGET)
client:
    $(CC) $(CFLAGS) udp-client.c -o client
server: 
    $(CC) $(CFLAGS) udp-server.c -o server
clean:
    $(RM) $(TARGET)
