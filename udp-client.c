/*
	udp-client: a demo of UDP/IP socket communication

	usage:	client [-h serverhost] [-p port]
*/

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
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>

#define BUF_SIZE 51200

struct frame_a {
	long int ID;
	long int len;
	char data[BUF_SIZE];
};


void printError( char *msg)
  { 
  perror(msg);
  exit(0);
  }
  
  
int main( int argc, char *argv[])
{
 
   if (argc!=3)
  { printf("USAGE: SERVER PORT\n");
    exit(1);
    }
    
    struct sockaddr_in send_addr, from_addr;
    struct stat st;
    struct frame_a frame;
    struct timeval t_out = {0, 0};
    
   
    long int n = 0;
    long int  len = 0;
    off_t f_size = 0;
    long int ack_num = 0;
    int sock, ack_recv = 0;
	
	
	
    char cmd_send[50];
    char filename[20];
    char cmd[10];
    char ack_send[4]="ACK";
    

    FILE *fptr;
    
    	memset(ack_send, 0, sizeof(ack_send));
	 bzero((char *) &from_addr, sizeof(from_addr));/*the client addr is the from address*/
	 bzero((char *) &send_addr, sizeof(send_addr));
        

   
	/*Populate send_addr structure with IP address and Port*/
	send_addr.sin_family = AF_INET;
	send_addr.sin_port = htons(atoi(argv[2]));
	send_addr.sin_addr.s_addr = inet_addr(argv[1]);

     sock= socket(AF_INET,SOCK_DGRAM,0);
  
   if (sock<0)
   {
    printError("opening socket");
   }
   
  while(1)
  {             
                memset(cmd_send, 0, sizeof(cmd_send));
		memset(cmd, 0, sizeof(cmd));
		memset(filename, 0, sizeof(filename));

		printf("\n Menu \n Enter any of the following commands \n   1.) get [file_name] \n 2.) put [file_name] \n 3.) delete [file_name] \n 4.) ls \n 5.) exit \n");		
		scanf(" %[^\n]%*c", cmd_send);
		
		
		printf("%s\n",cmd_send);
		
		
		sscanf(cmd_send, "%s %s", cmd, filename);		//parse the user input into command and filename

		if (sendto(sock, cmd_send, sizeof(cmd_send), 0, (struct sockaddr *) &send_addr, sizeof(send_addr)) == -1) {
			printError("Client: send");
		}
/*----------------------------------------------------------------------"get case"-------------------------------------------------------------------------*/
    if((strcmp(cmd,"get")==0) && (filename[0] != '\0' )) 
    
    {
    
    long int total_frame=0;
    long int bytes_rec=0,i=0;
   
     
     t_out.tv_sec = 2;
			setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&t_out, sizeof(struct timeval));

                        recvfrom(sock, &(total_frame), sizeof(total_frame), 0, (struct sockaddr *) &from_addr, (socklen_t *) &len);
   
       t_out.tv_sec = 0;
                	setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&t_out, sizeof(struct timeval));
                	
  if (total_frame > 0) {
				
                        sendto(sock, &(total_frame),sizeof(total_frame), 0, (struct sockaddr *) &send_addr, sizeof(send_addr));
                         printf("----> %ld\n", total_frame);              	
 

fptr = fopen(filename, "wb");

for (i=1; i<=total_frame;i++)
{
   memset((&frame),0,sizeof(frame));
   
   recvfrom(sock,&(frame),sizeof(frame),0,(struct sockaddr*)&from_addr, (socklen_t *)&len);
   
   sendto(sock,&(frame.ID),sizeof(frame.ID),0,(struct sockaddr*)&send_addr, sizeof(send_addr));
   
  if(frame.ID!=i){
      i--;   }
  
  else 
     {
     fwrite(frame.data,1,frame.len,fptr);
     printf("The frame ID of the received frame is %ld ...and the frame length is %ld\n", frame.ID, frame.len);
     
     bytes_rec+=frame.len;
     }
     
    
     if(i==total_frame)
     {printf("THE FILE HAS BEEN SUCCESSFULLY RECEIVED\n");
     
     }
     }
    
     printf("Total bytes received is %ld",bytes_rec);
     fclose(fptr);
   }
     else 
     {
     printf("The file is empty\n");
     }
   
}
/*PUT CASE --------------------------------------------------------------------*/

    else if((strcmp(cmd,"put")==0) && (filename[0] != '\0' )) 

{   if (access(filename, F_OK)==0){

   int total_frame=0, resend_frame= 0, drop_frame=0,t_out_flag=0;
   long int i=1; 
   
   stat(filename ,&st);
   f_size=st.st_size;
   
   printf("The size of the file is %ld\n", f_size);
   
   t_out.tv_sec=6;
   t_out.tv_usec=0;
   
   setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&t_out, sizeof(struct timeval));
   
   fptr = fopen(filename, "rb");
   
   if((f_size % BUF_SIZE) !=0)
     { total_frame = (f_size/BUF_SIZE)+1;}
   else 
      total_frame= (f_size/BUF_SIZE);
      
      printf("THe total frame size is %ld\n",total_frame);
      
      sendto(sock, &(total_frame),sizeof(total_frame), 0, (struct sockaddr *) &send_addr, sizeof(send_addr));
      
      recvfrom(sock, &(ack_num),sizeof(ack_num), 0, (struct sockaddr *) &from_addr, (socklen_t *) &len);

/* sending the acknowledgement */

printf("The acknowledgement number is %ld \n", ack_num);
 
   if(ack_num != total_frame)
  {
    sendto(sock, &(total_frame),sizeof(total_frame), 0, (struct sockaddr *) &send_addr, sizeof(send_addr));
      
      recvfrom(sock, &(ack_num),sizeof(ack_num), 0, (struct sockaddr *) &from_addr, (socklen_t *)&len);
      
      resend_frame++;
      
      if(resend_frame==20)
      { 
      
      t_out_flag=1;
      break;
      
      }
}
   for(i=1; i<= total_frame; i++)
    { 
      //printf("INSIDE THE FOR LOOP");
      memset((&frame),0,sizeof(frame));
      frame.ID=i;
      frame.len= fread(frame.data,1,BUF_SIZE,fptr);
      
      sendto(sock, &(frame),sizeof(frame), 0, (struct sockaddr *) &send_addr, sizeof(send_addr));
      
      //printf("THE FRAMES HAVE BEEN SENT PLIS");
      
      recvfrom(sock, &(ack_num),sizeof(ack_num), 0, (struct sockaddr *) &from_addr, (socklen_t *)&len);
      
 while (ack_num!=frame.ID)
 {   sendto(sock, &(frame),sizeof(frame), 0, (struct sockaddr *) &send_addr, sizeof(send_addr));
      
      
      recvfrom(sock, &(ack_num),sizeof(ack_num), 0, (struct sockaddr *) &from_addr, (socklen_t *)&len);
      
      resend_frame++;
      
      if(resend_frame==200)
     { t_out_flag=1;
      break;}
      }
      
      resend_frame=0;
      drop_frame=0;
      
      if(t_out_flag==1)
      {
      printf("FILE NOT SENT \n");
      break;
      
        }
        
        if(total_frame==ack_num)
        {printf("FILE SENT \n");
        }
      }

  fclose(fptr);
  

printf("Disabling the time out");
t_out.tv_sec=0;
setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&t_out, sizeof(struct timeval));
}
}
/*---------------------ls case---------------------------------------*/
 else if((strcmp(cmd,"ls")==0))
 
 {  char file_is[200];
    memset((file_is),0,sizeof(file_is));
    
    len= sizeof(from_addr);
    n = recvfrom(sock, file_is,sizeof(file_is), 0, (struct sockaddr *) &from_addr, (socklen_t *)&len);
    
    //printf("the value of n is %ld\n ",n);
    //printf("\nThe files received are-> %s\n",file_is);
    
    //if(n<0)
    //{
    //printf("Error in the receive of ls"); }
    
    //if(strcmp(file_is,"")!=0)
    
       printf("No of bytes received %ld \n",n);
        
       printf("\nThe files received are-> %s\n",file_is);  
       
     
          
          //}
     //else 
     //{
     //printf("No files have been received");
       //continue;
      
     //}
    }
    
/*--------------------------------delete case------------------------*/

  else if((strcmp(cmd,"delete")==0) && strcmp(filename,"")!=0) 
  {
     len =sizeof(from_addr);
     ack_recv=0;
     
     
     if (access(filename, F_OK)==0){
     
   n = recvfrom(sock, ack_recv,sizeof(ack_recv), 0, (struct sockaddr *) &from_addr, (socklen_t *)&len);
   
  
   
   //if(n<0)
     // {printf("Error in ack of files to be deleted");
    //}
    
    if(n=1)
       
     {  printf("CLIENT: THe file has been deleted");}
     
     
     else 
     printf("filename is invalid");
       }
    
      
       else 
         
         printf("CLIENT: File does not have the appropriate permission or the file does not exist \n");
        } 
  /*------------------------------exit case--------------*/
  
   else if((strcmp(cmd,"exit")==0))
   {
   
   exit(EXIT_SUCCESS);
   
   }      
   
else {

printf("INVALID COMMAND \n");


}

}

close (socket);

exit(EXIT_SUCCESS);
}   
   
 







   
  
 
   
   
