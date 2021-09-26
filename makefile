all: client server

client:
       g++ -o client udp-client.c

server:
       g++ -o server udp-server.c
