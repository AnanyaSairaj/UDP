CC =cc
normal: $(TARGET)
client:  
	gcc -Wall -Werror udp-client.c -o client
server: 
	gcc -Wall -Werror udp-server.c -o server
clean:
	$(RM) $(TARGET)		

