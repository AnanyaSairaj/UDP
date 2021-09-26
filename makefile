CC = cc

CFLAGS  = -g -Wall -Werror 

TARGET = server client 

all: $(TARGET)
	
server:$(SERVER)/udp-server.c
	$(CC) $(CFLAGS) -o server udp-server.c 

client:$(CLIENT)/udp-client.c 
	$(CC) $(CFLAGS) -o client udp-client.c 

clean:
	$(RM) $(TARGET)
