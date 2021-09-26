# UDP
 Implementation of file transfer in UDP with reliability
A client and server implementation of UDP-based file transfer. The protocol used borrows from [TFTP](https://en.wikipedia.org/wiki/Trivial_File_Transfer_Protocol) and [FTP](https://en.wikipedia.org/wiki/File_Transfer_Protocol) and provides reliable file transfer.

## Commands
Five commands have been implemented: 
* GET - Client requests a file,server transmits the file the requested file.
* PUT - Client transmits a file ,server receives it and stores it locally.
* LS -Server searches for all the files in its local dirctory and sends it as a list to the client.
* DELETE -The server deletes the specified file.
* EXIT-Both exit the loop.

## Reliability Implementation:
To implement the reliabilty required of us to the inherently unreliable UDP, we use the infamous STOP-AND-WAIT protocol. Though it is time consuming, it is simple and robust to implemenent. To explain the implementation let us take the example of the get case.
1-In the get part the client side makes a request to the server to get the file. The server side receives the request and first calculates the number of frames/packets to be sent by dividing the overall file size with the buffer size.
2-After it gets the total number of frames to be sent, it sends that to the client which sends an acknowledgement back.It then checks if the acknowledgement received is valid and if not valid it retransmits till it hits a match or crosses the limit for retransmission. 
3-Then it transmit the frame one by one with an iteration comparing it to the total frames calculated ,receives the acknowledgement and retransmits if the frame id does not match.
4-It breaks out of the loop if the number of time resent exceeds the limit specified.
5-If the total number of frames have been sent and the acknowledgemnts for them have been received then it displays a "SUCCESSFULLY SENT" message.

6- Vice versa is applicable for the put case

This is how basic reliablility is implemented for the file transfer using the  STOP-AND-WAIT protocol.

## Instructions
The makefile runs a phony recursive loop to the subdirectories to compile them. Running the following command will take you into the specified subdirectories and run their respective makefiles and hence create the executables.
```
make
```
Once the compilation is done, the server and the client executables can be run in the following manner:

### Server
```
cd [SERVER]
./server [PORT NUMBER]
```
*Port Number* must be greater than 5000.
### Client
```
cd [CLIENT]
./[Server IP Address] [Server Port Number]
```
### Running commands on the client

## Results
The client and server have been tested to reliably transfer files up to 100 MB in a 1% packet loss environment.

## Authors
* Ananya Sairaj
## License


