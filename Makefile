CC =cc
normal: $(TARGET)
client:$(CLIENT)/udp-client.c  
	gcc -Wall -Werror $(CLIENT)/udp-client.c -o client
server: $(SERVER)/udp-server.c 
	gcc -Wall -Werror $(SERVER)/udp-server.c -o server
clean:
	$(RM) $(TARGET)		

