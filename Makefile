CC =cc
normal: $(TARGET)
client:  
	gcc -Wall -Werror $(CLIENT)/udp-client.c -o client
server: $ 
	gcc -Wall -Werror $(SERVER)/udp-server.c -o server
clean:
	$(RM) $(TARGET)		

