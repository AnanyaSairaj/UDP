
#include <stdio.h>
#include <stdlib.h>	/* needed for os x*/
#include <stdarg.h>
#include <string.h>	/* for strlen */
#include <netdb.h>      /* for gethostbyname() */
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>	/* defines inet_ntoa */
#include <netinet/in.h>
#include <sys/errno.h>   /* defines ERESTART, EINTR */
#include <sys/wait.h>    /* defines WNOHANG, for wait() */
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>

#define BUF_SIZE 51200

struct frame_b{
	long int ID;
	long int length;
	char data[BUF_SIZE];
};

int ls(FILE *fptr) 
{ 
	struct dirent **dirent; int n = 0;

	if ((n = scandir(".", &dirent, NULL, alphasort)) < 0) { 
		perror("Scanerror"); 
		return -1; 
	}
        
	while (n--) {
		fprintf(fptr, "%s\n", dirent[n]->d_name);	
		free(dirent[n]); 
	}
	
	free(dirent); 
	return 0; 
}         
void printError( char *msg)
  { 
  perror(msg);
  exit(0);
  }
  
int main( int argc, char **argv[])
{
    if (argc<2)
  { fprintf(stderr,"No port error\n");
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
   
   ssize_t n;
   ssize_t length;
   off_t filesize;
   long int ack_num=0;
   int ack_send=0;
   int sock;
   
   FILE *fptr;
   length= sizeof(server);
   bzero(&server,length);
   
   server.sin_family= AF_INET;
   server.sin_addr.s_addr=INADDR_ANY;
   server.sin_port=htons(atoi(argv[1]));
   
   sock= socket(AF_INET,SOCK_DGRAM,0);
   
   if(sock<0)
   
   {printError("Server side socket error");}
   if(bind(sock,(struct sockaddr *)&server,length)<0)
   {printError("Binding");
   }
   
   // printf("BINDING SUCCESSFULL");
   //fromlen=sizeof( struct sockaddr_in);
   
   while(1)
   
    {  
        printf("Waiting for the client to connect\n");
       
       
       memset(msgreceive,0,sizeof(msgreceive));
       memset(cmdreceive,0,sizeof(cmdreceive));
       memset(filenamereceive,0,sizeof(filenamereceive));
       
       length=sizeof(client);
      
    
       n=recvfrom(sock,msgreceive,BUF_SIZE,0,(struct sockaddr *)&client,(socklen_t *)&length);
      
      if (n<0)
      {  
          printError("Server:recieve issue");
          }
       else 
       
       {printf("Server: The received message is %s,\n",msgreceive);}
       
       sscanf(msgreceive,"%s %s",cmdreceive,filenamereceive);
       
       
 /* ---------------------------------PUT CASE-----------------------------*/
      if ((strcmp(cmdreceive, "put") == 0) && (filenamereceive[0] != '\0' ))
      
      //strcmp(filenamereceive, "") != 0)
        
   { 
   //printf("INSIDE THE PUT LOOP");
   
   printf("Server: TO put the file named ->%s\n",filenamereceive);
   
   long int total_frame=0;
   ;long int bytesreceive=0;
   long int i=0;
   
   t_out.tv_sec =2;
   int f=0;
   setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&t_out, sizeof(struct timeval));
     
  f= recvfrom(sock, &(total_frame), sizeof(total_frame), 0, (struct sockaddr *) &client, (socklen_t *) &length);
  
  t_out.tv_sec=0;
  
  setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&t_out, sizeof(struct timeval));
  //printf("the value of f is %ld",f);
 
  
  if(f>0)
    {  sendto(sock, &(total_frame),sizeof(total_frame), 0, (struct sockaddr *) &client, sizeof(client));
                   printf("The total number of frames are %ld\n", total_frame);              	   

      fptr = fopen(filenamereceive, "wb");
      
      //printf("file opened in write mode");
      
      for(i=1;i<= total_frame; i++)
      {
      memset(&frame,0,sizeof(frame));
      
      recvfrom(sock, &(frame), sizeof(frame), 0, (struct sockaddr *) &client, (socklen_t *) &length);
      
      sendto(sock, &(frame.ID), sizeof(frame.ID), 0, (struct sockaddr *) &client, sizeof(client));
      
      //printf("has the frame been received?\n");
      if(frame.ID<i||frame.ID>i)
      {
      i--;
      
      }
      
      else
      {
      
       fwrite(frame.data,1,frame.length,fptr);
     printf("The frame ID of the received frame is %ld and the frame length is %ld\n", frame.ID, frame.length);
     
     bytesreceive +=frame.length;
     
      }
      if(i==total_frame)
          printf("THE FILE HAS BEEN RECEIVED SUCCESSFULLY.\n");
      }
      
      printf("Total bytes received is %ld\n",bytesreceive);
      
      fclose(fptr);
      }
     
      else
      {printf("File is empty\n");
      }
      
      }
   
   
/*------------------------------GET CASE-----------------------------*/

  else if ((strcmp(cmdreceive,"get")==0)&& (strcmp(filenamereceive,"")!=0))
 
  {
  
  printf("Server: To GET file named %s\n",filenamereceive);
  
  if(access(filenamereceive,F_OK)==0)
  {   
    int total_frame=0, resend_frame=0, drop_frame=0, t_out_flag=0;
    long int i=0;
    
    fptr=fopen(filenamereceive,"rb");
    fseek(fptr,0L,SEEK_END);
    long int f_size= ftell(fptr);
    rewind(fptr);
   printf("The file size is %ld\n",f_size);
    t_out.tv_sec=6;
    t_out.tv_usec=0;
    
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&t_out, sizeof(struct timeval));   
   
    fptr=fopen(filenamereceive,"rb");
  
    if((f_size % BUF_SIZE)!=0)
     total_frame=(f_size/BUF_SIZE)+1;
     else
     total_frame=(f_size/BUF_SIZE);
     
     printf("The total number of frames sent %d\n",total_frame);
     
     length=sizeof(client);
     
     sendto(sock, &(total_frame),sizeof(total_frame), 0, (struct sockaddr *) &client, sizeof(client));
       recvfrom(sock, &(ack_num), sizeof(ack_num), 0, (struct sockaddr *) &client, (socklen_t *) &length);
       
       while(ack_num !=total_frame){
       
       sendto(sock, &(total_frame),sizeof(total_frame), 0, (struct sockaddr *) &client, sizeof(client));
       
      recvfrom(sock, &(ack_num), sizeof(ack_num), 0, (struct sockaddr *) &client, (socklen_t *) &length);
      resend_frame++;
      
      if(resend_frame==20){
      
      t_out_flag==1;
      break;
      }
       }
       
       for(i=1;i<=total_frame;i++)
       { memset(&frame,0,sizeof(frame));
         ack_num=0;
         frame.ID=i;
         frame.length=fread(frame.data,1,BUF_SIZE,fptr);
         
          sendto(sock, &(frame),sizeof(frame), 0, (struct sockaddr *) &client, sizeof(client));
         
         
         recvfrom(sock, &(ack_num), sizeof(ack_num), 0, (struct sockaddr *) &client, (socklen_t *) &length);
         
         while(ack_num!=frame.ID)
         
         {  sendto(sock, &(frame),sizeof(frame), 0, (struct sockaddr *) &client, sizeof(client));
         
         recvfrom(sock, &(ack_num), sizeof(ack_num), 0, (struct sockaddr *) &client, (socklen_t *) &length);
         
         printf("FRAME %ld has been dropped %d times\n",frame.ID,++drop_frame);
         
         resend_frame++;
         
         if(resend_frame==200)
         {
           t_out_flag=1;
           break;
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
           
           }
           fclose(fptr);
           
           t_out.tv_sec=0;
           t_out.tv_usec=0;
            setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&t_out, sizeof(struct timeval)); 
            printf("Timeout has been disabled\n");
            }}
            else 
            {
            printf("INVALID FILENAME\n");
            }
            }
                      
/*----------------------------------LS---------------------------------*/

      else if((strcmp(cmdreceive,"ls")==0))
     {
       char file_entry[200];
       memset(file_entry,0,sizeof(file_entry));
       
       int n=0;
       
       fptr=fopen("ls.log","wb");
        
        if(ls(fptr) ==-1)
        printError("ls");
        
        fclose(fptr);
        
           fptr= fopen("ls.log","rb");
           int filesize = fread(file_entry,1,200,fptr);
           
           printf("Filesize= %d %ld\n", filesize,strlen(file_entry));
           
          n= sendto(sock,file_entry,filesize, 0, (struct sockaddr *) &client, sizeof(client));
          printf("\nThe files sent are-> %s\n",file_entry);
          if(n<0)
              { printError("SERVER: send");}
               
               remove("ls.log");
               fclose(fptr);
               
               }
               
        
/*-----------------------------DELETE CASE----------------------------*/
 else if ((strcmp(cmdreceive,"delete")==0) && (strcmp(filenamereceive,"")!=0))
 {
 
  if((access(filenamereceive,F_OK)==0) && (access(filenamereceive,R_OK)==0))
            
  { printf("The name of the file to be deleted is %s\n",filenamereceive);
   remove(filenamereceive);
   ack_send=1;
   sendto(sock, &(ack_send),sizeof(ack_send), 0, (struct sockaddr *) &client, sizeof(client));
   printf("The sent is %ld",ack_send);
   
   }
   else
   {
      ack_send =-1;
      
      sendto(sock, &(ack_send),sizeof(ack_send), 0, (struct sockaddr *) &client, sizeof(client));
     // printf("why is ack_send -1%ld",ack_send);
  
   }}
   
          
  
//-------------------------------------EXIT CASE--------------------*/
  
  else if (strcmp(cmdreceive,"exit")==0){
  
  close(socket);
  exit(EXIT_SUCCESS);
  }
  
  
  else 
  {
  printf("Server: Unknown command.Please enter valid command\n");
  
  }
  
  }
  close(socket);
  exit(EXIT_SUCCESS);
  } 
