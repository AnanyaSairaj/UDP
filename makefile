all: client server

client:
       g++ -o client client.c

server:
       g++ -o server server.c
