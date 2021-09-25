
#include <stdio.h>
#include <stdlib.h>	
#include <stdarg.h>
#include <string.h>	
#include <netdb.h>      
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>	
#include <netinet/in.h>
#include <sys/errno.h>   
#include <sys/wait.h>    
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>

#define BUF_SIZE 51200//define large buffer size for 100MB files

struct frame_b{  //declaring frame structure with ID len and buffer size
	long int ID;
	long int length;
	char data[BUF_SIZE];
};

int ls(FILE *fptr) 
{ 
	struct dirent **dirent; int n = 0;//dirent pointers declared to use DIRENT objects

	if ((n = scandir(".", &dirent, NULL, alphasort)) < 0) { 
		perror("Scanerror"); //declare error if problem in scanning the directories
		return -1;//return -1 for error 
	}
        
	while (n--) {
		fprintf(fptr, "%s\n", dirent[n]->d_name);//print the directory names 	
		free(dirent[n]); 
	}
	
	free(dirent); 
	return 0; 
}         
void printError( char *msg)//declare the errors
  { 
  perror(msg);
  exit(0);
  }
  
int main( int argc, char **argv[])//accept the arguments
{
    if (argc<2)
  { fprintf(stderr,"No port error\n");//if arguments are not valid print errors
    exit(1);
    }
    
   struct sockaddr_in server;
   struct sockaddr_in client;
   struct stat st;
   struct frame_b frame;
   struct timeval t_out = {0, 0};
 
 
 
   
   char msgreceive[BUF_SIZE];
   char filenamereceive[30];
   char cmdreceive[20];
   
   ssize_t n;//ssizet for non zero values 
   ssize_t length;
   off_t filesize;
   long int ack_num=0;
   int ack_send=0;
   int sock;
   
   FILE *fptr;//initialize the file pointer 
   length= sizeof(server);
   bzero(&server,length);
   
   //Populate send_addr structure with IP address and Port
   server.sin_family= AF_INET;
   server.sin_addr.s_addr=INADDR_ANY;
   server.sin_port=htons(atoi(argv[1]));
   
   sock= socket(AF_INET,SOCK_DGRAM,0);//binding the socket and checking for errors
   
   if(sock<0)
   
   {printError("Server side socket error");}
   if(bind(sock,(struct sockaddr *)&server,length)<0)
   {printError("Binding");
   }
   
   // printf("BINDING SUCCESSFULL");
   //fromlen=sizeof( struct sockaddr_in);
   
   while(1)//infinite while loop
   
    {  
        printf("Waiting for the client to connect\n");
       
       
       memset(msgreceive,0,sizeof(msgreceive));//clearing the memory 
       memset(cmdreceive,0,sizeof(cmdreceive));
       memset(filenamereceive,0,sizeof(filenamereceive));
       
       length=sizeof(client);//initialiize the length as the size of client
      
    
       n=recvfrom(sock,msgreceive,BUF_SIZE,0,(struct sockaddr *)&client,(socklen_t *)&length);//get the message
      
      if (n<0)
      {  
          printError("Server:recieve issue");
          }
       else 
       
       {printf("Server: The received message is %s,\n",msgreceive);}
       
       sscanf(msgreceive,"%s %s",cmdreceive,filenamereceive);//parse for the command and the filename 
       
       
 /* ---------------------------------PUT CASE-----------------------------*/
      if ((strcmp(cmdreceive, "put") == 0) && (filenamereceive[0] != '\0' ))
      
      //strcmp(filenamereceive, "") != 0)
        
   { 
   //printf("INSIDE THE PUT LOOP");
   
   printf("Server: To PUT the file named ->%s\n",filenamereceive);
   
   long int total_frame=0;
   long int bytesreceive=0;
   long int i=0;
   
   t_out.tv_sec =6;//set the timeout value to larger number for 100MB file transfer
   int f=0;
   setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&t_out, sizeof(struct timeval));
     
  f= recvfrom(sock, &(total_frame), sizeof(total_frame), 0, (struct sockaddr *) &client, (socklen_t *) &length);
  
  t_out.tv_sec=0;
  
  setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&t_out, sizeof(struct timeval));
  //printf("the value of f is %ld",f);
 
  
  if(f>0)
    {  sendto(sock, &(total_frame),sizeof(total_frame), 0, (struct sockaddr *) &client, sizeof(client));//send the total frame back to check
                   printf("The total number of frames are %ld\n", total_frame);              	   

      fptr = fopen(filenamereceive, "wb");//open the file in write mode
      
      //printf("file opened in write mode");
      
      for(i=1;i<= total_frame; i++)//send frames one by one
      {
      memset(&frame,0,sizeof(frame));
      
      recvfrom(sock, &(frame), sizeof(frame), 0, (struct sockaddr *) &client, (socklen_t *) &length);//receive the frame
      
      sendto(sock, &(frame.ID), sizeof(frame.ID), 0, (struct sockaddr *) &client, sizeof(client));//send the frame id as acknowledgement
      
      //printf("has the frame been received?\n");
      if(frame.ID<i||frame.ID>i)//check if the correct frame id has been recieve if not go to the required i-th iteration
      {
      i--;
      
      }
      
      else
      {
      
       fwrite(frame.data,1,frame.length,fptr);//write the data into the file
     printf("The frame ID of the received frame is %ld and the frame length is %ld\n", frame.ID, frame.length);
     
     bytesreceive +=frame.length;//increment the bytes received according to the frame length
     
      }
      if(i==total_frame)
          printf("THE FILE HAS BEEN RECEIVED SUCCESSFULLY.\n");
      }
      
      printf("Total bytes received is %ld\n",bytesreceive);
      
      fclose(fptr);//close the pointer
      }
     
      else
      {printf("File is empty\n");
      }
      
      }
   
   
/*------------------------------GET CASE-----------------------------*/

  else if ((strcmp(cmdreceive,"get")==0)&& (strcmp(filenamereceive,"")!=0))
 //check for the get case
  {
  
  printf("Server: To GET file named %s\n",filenamereceive);
  
  if(access(filenamereceive,F_OK)==0)//check the access priviledges of the file 
  {   
    int total_frame=0, resend_frame=0, drop_frame=0, t_out_flag=0;
    long int i=0;
    
    fptr=fopen(filenamereceive,"rb");
    fseek(fptr,0L,SEEK_END);
    long int f_size= ftell(fptr);//get the file size
    rewind(fptr);//rewind the pointer back to the start of the file
   printf("The file size is %ld\n",f_size);
    t_out.tv_sec=6;//setting the timeout value
    t_out.tv_usec=0;
    
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&t_out, sizeof(struct timeval));   
   
    fptr=fopen(filenamereceive,"rb");
  
    if((f_size % BUF_SIZE)!=0)
     total_frame=(f_size/BUF_SIZE)+1;//calculating the total frames by dividing the file size with the buffer and adding +1 if rem is not 0 and hence frame size is not whole
     else
     total_frame=(f_size/BUF_SIZE);
     
     printf("The total number of frames sent %d\n",total_frame);
     
     length=sizeof(client);
     
     sendto(sock, &(total_frame),sizeof(total_frame), 0, (struct sockaddr *) &client, sizeof(client));
       recvfrom(sock, &(ack_num), sizeof(ack_num), 0, (struct sockaddr *) &client, (socklen_t *) &length);//receive the acknum
       
       while(ack_num !=total_frame){//check if the total frame and ack num match ,if not ,resend it
       
       sendto(sock, &(total_frame),sizeof(total_frame), 0, (struct sockaddr *) &client, sizeof(client));
       
      recvfrom(sock, &(ack_num), sizeof(ack_num), 0, (struct sockaddr *) &client, (socklen_t *) &length);
      resend_frame++;//increment the resend value for each time it is resent
      
      if(resend_frame==20){
      
      t_out_flag==1;//set flag as 1 when it reaches the limit and break
      break;
      }
       }
       
       for(i=1;i<=total_frame;i++)//run for loop to send frames one by one
       { memset(&frame,0,sizeof(frame));
         ack_num=0;
         frame.ID=i;
         frame.length=fread(frame.data,1,BUF_SIZE,fptr);
         
          sendto(sock, &(frame),sizeof(frame), 0, (struct sockaddr *) &client, sizeof(client));//send the frame
         
         
         recvfrom(sock, &(ack_num), sizeof(ack_num), 0, (struct sockaddr *) &client, (socklen_t *) &length);//receive the acknowledgement
         
         while(ack_num!=frame.ID)//if the acknowledgement has not been matched resend  the frame
         
         {  sendto(sock, &(frame),sizeof(frame), 0, (struct sockaddr *) &client, sizeof(client));
         
         recvfrom(sock, &(ack_num), sizeof(ack_num), 0, (struct sockaddr *) &client, (socklen_t *) &length);
         
         //printf("FRAME %ld has been dropped %d times\n",frame.ID,++drop_frame);
         
         resend_frame++;//increment the resend value
         
         if(resend_frame==200)
        { printf("the program is breaking here");
           t_out_flag=1;//set the flag as 1 when resend reaches limit
           break;//break out of the loop
           }
           }
           
           resend_frame=0;
           drop_frame=0;
           
           if(t_out_flag==1)
           {
           printf("File was not sent\n");
           
           break;
           }
           printf("The frame sent is %ld and the acknowledgement is %ld\n",i,ack_num);
           
           if(ack_num==total_frame) 
           {
             printf("THE FILE HAS BEEN SENT SUCCESSFULLY\n");
           
           }}
           fclose(fptr);
           
           t_out.tv_sec=0;//diable the time out 
           t_out.tv_usec=0;
            setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&t_out, sizeof(struct timeval)); 
            printf("Timeout has been disabled\n");
            }
            else 
            {
            printf("INVALID FILENAME\n");
            }
            }
                      
/*----------------------------------LS---------------------------------*/

      else if((strcmp(cmdreceive,"ls")==0))//compare the command received to ls and go into the loop
     {
       char file_entry[200];//initalize a string and empty it
       memset(file_entry,0,sizeof(file_entry));
       
       int n=0;
       
       fptr=fopen("ls.log","wb");//open temporary file in write mode
        
        if(ls(fptr) ==-1)//if the acknowledgement received is negative print the error
        printError("ls");
        
        fclose(fptr);
        
           fptr= fopen("ls.log","rb");//open temporary file in read mode
           int filesize = fread(file_entry,1,200,fptr);//read the file entries
           
           printf("Filesize= %d %ld\n", filesize,strlen(file_entry));
           
          n= sendto(sock,file_entry,filesize, 0, (struct sockaddr *) &client, sizeof(client));//send the files
          
          printf("\nThe files sent are-> %s\n",file_entry);
          if(n<0)
              { printError("SERVER: send");}
               
               remove("ls.log");//remove the temporary file
               fclose(fptr);//close the pointer
               
               }
               
        
/*-----------------------------DELETE CASE----------------------------*/
 else if ((strcmp(cmdreceive,"delete")==0) && (strcmp(filenamereceive,"")!=0))
 {
 
  if((access(filenamereceive,F_OK)==0) && (access(filenamereceive,R_OK)==0))//check for access priviledges
            
  { printf("The name of the file to be deleted is %s\n",filenamereceive);
   remove(filenamereceive);
   ack_send=1;//send positive ack on removal
   sendto(sock, &(ack_send),sizeof(ack_send), 0, (struct sockaddr *) &client, sizeof(client));
   //printf("The sent is %ld",ack_send);
   
   }
   else
   {
      ack_send =-1;//else send negative acknowledgement
      
      sendto(sock, &(ack_send),sizeof(ack_send), 0, (struct sockaddr *) &client, sizeof(client));
     // printf("why is ack_send -1%ld",ack_send);
  
   }}
   
          
  
//-------------------------------------EXIT CASE--------------------*/
  
  else if (strcmp(cmdreceive,"exit")==0){
  
  close(socket);
  exit(EXIT_SUCCESS);//successfully exits 
  }
  
  
  else 
  {
  printf("Server: Unknown command.Please enter valid command\n");
  
  }
  
  }
  close(socket);
  exit(EXIT_SUCCESS);
  } 
