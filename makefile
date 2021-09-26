CC = cc

CFLAGS  = -g -Wall  

TARGET = udp-server udp-client 

all: $(TARGET)
	
server:(SERVER)/udp-server.c
	$(CC) $(CFLAGS) -o udp-server udp-server.c 

client:(CLIENT)/udp-client.c 
	$(CC) $(CFLAGS) -o udp-client udp-client.c 

clean:
	$(RM) $(TARGET)
