TARGET= client server 
normal: $(TARGET)
client: udp-client.c
    gcc -Wall udp-client.c -o client
server: udp-server.c
    gcc -Wall udp-server.c -o server
clean:
    $(RM) $(TARGET)
