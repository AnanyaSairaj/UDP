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
To implement the reliabilty required of us to the inherently unreliable UDP, we use the infamous STOP-AND-WAIT protocol. Though it is time consuming, it is simple and robust to implemenent. To explain the implementation let us take the example of the get function. The client side makes a request to the server to get the file. The server side receives the request and first calculates the number of frames/packets to be sent by dividing the overall file size with the buffer size. After it gets the total number of frames to be sent, it sends that to the client which sends an acknowledgement back.It then checks if the acknowledgement received is valid and if not valid it retransmits till it hits a match or crosses the limit for retransmission. Then it transmit the frame one by one with an iteration comparing it to the total frames calculated ,receives the acknowledgement and retransmits if the frame id does not match.The vice versa is applicable for put. This is how the stop and wait protocol is implemented.

## Instructions
The makefile stores object files in *obj/* and executables in [run](https://github.com/nimbid/Reliable-UDP-Based-File-Transfer/tree/main/run). Run the following command in the directory where you clone this repo to compile the code.
```
make
```
Once the compilation is done, the server and the client executables can be run in the following manner:
### Server
```
./filepath/run/uftp_server [Port Number] 
```
*Port Number* must be greater than 5000.
### Client
```
./filepath/run/uftp_client [Server IP Address] [Server Port Number]
```
### Running commands on the client
**NOTE**: In the above commands, 'filepath' must be replaced by the path on your system, based on your current directory. This is especially important for the *LS* command as the server lists its current directory based on where it is running (where it was called from).
## Results
The client and server have been tested to reliably transfer files up to 100 MB in a 1% packet loss environment.
### Creating large files for testing
The following command can be run to create an arbitrary file of a size of your choosing.
```
dd if=/dev/zero of=testfile bs=1024 count=102400
```
The variables *bs* and *count* can be modified to create a file size as large or small as you want.
## Authors
* Nimish Bhide
## License
[MIT](https://choosealicense.com/licenses/mit/)

