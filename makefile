CC = cc

CFLAGS  = -g -Wall  

TARGET = udp-server udp-client 

all: $(TARGET)
	
udp-server: udp-server.c
	$(CC) $(CFLAGS) -o udp-server udp-server.c 

udp-client: udp-client.c
	$(CC) $(CFLAGS) -o udp-client udp-client.c 

clean:
	$(RM) $(TARGET)
