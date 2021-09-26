TARGET= client server 
normal: $(TARGET)
client: client.c
    gcc -Wall udp-client.c -o client
server: server.c
    gcc -Wall udp-server.c -o server
clean:
    $(RM) $(TARGET)
