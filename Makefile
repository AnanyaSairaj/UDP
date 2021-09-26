CC =cc
normal: $(TARGET)
client: udp-client.c 
	gcc -Wall -Werror udp-client.c -o client
server: udp-server.c
	gcc -Wall -Werror udp-server.c -o server
clean:
	$(RM) $(TARGET)		

