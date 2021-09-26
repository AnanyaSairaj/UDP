CC = cc

CFLAGS  = -g -Wall -Werror 

TARGET = server client 

all: $(TARGET)
	
server:
	$(CC) $(CFLAGS) -o server $(SERVER)/udp-server.c 

client:$ 
	$(CC) $(CFLAGS) -o client $(CLIENT)/udp-client.c 

clean:
	$(RM) $(TARGET)
