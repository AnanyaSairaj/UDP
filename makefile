CC = cc

CFLAGS  = -g -Wall  

TARGET = udp-server udp-client 

all: $(TARGET)
	
udp-server: 
	$(CC) $(CFLAGS) -o udp-server udp-server.c 

udp-client: 
	$(CC) $(CFLAGS) -o udp-client udp-client.c 

clean:
	$(RM) $(TARGET)
