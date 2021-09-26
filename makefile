CC = cc

CFLAGS  = -g -Wall -Werror 

TARGET = server client 

all: $(TARGET)
	
server:$(SERVER)/udp-server.c
	$(CC) $(CFLAGS) -o server $(SERVER)/udp-server.c 

client:$(CLIENT)/udp-client.c 
	$(CC) $(CFLAGS) -o client $(CLIENT)/udp-client.c 

clean:
	$(RM) $(TARGET)
