//Socket server
#include <sys/types.h>
#include<sys/socket.h>
#include<netdb.h>
#include<unistd.h> //for read/write
#include<string.h> //for memset
#include<stdio.h> //for fprintf, stderr, etc.
#include<stdlib.h> //for exit


#include <sys/ioctl.h>
#include <sys/poll.h>
#include <errno.h>

int main()
{
  int sockfd; //file descriptor for the socket
  int status; //for error checking

  /*
    Changes to use poll

  */
  struct pollfd fds[5];


  //change this # between 2000-65k before using
  const char* portno="42425";
  struct addrinfo hints;
  memset(&hints, 0, sizeof(hints)); //zero out everything in structure
  hints.ai_family = AF_UNSPEC; //don't care. Either IPv4 or IPv6
  hints.ai_socktype=SOCK_STREAM; // TCP stream sockets
  hints.ai_flags=AI_PASSIVE; //file in the IP of the server for me

  struct addrinfo *servinfo;
  if((status=getaddrinfo(NULL, portno, &hints, &servinfo))==-1)
  {
    fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
    exit(1);
  }
  sockfd=socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);

  /*avoid "Address already in use" error*/
  int yes=1;
  if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int))==-1)
  {
    perror("setsockopt");
    exit(1);
  }

  //We need to "bind" the socket to the port number so that the kernel
  //can match an incoming packet on a port to the proper process
  if((status=bind(sockfd, servinfo->ai_addr, servinfo->ai_addrlen))==-1)
  {
    perror("bind");
    exit(1);
  }
  //when done, release dynamically allocated memory
  freeaddrinfo(servinfo);

  if(listen(sockfd,1)==-1)
  {
    perror("listen");
    exit(1);
  }


  int new_sockfd;
  //Initialize the poll
  memset(fds, 0 , sizeof(fds));

  for(int i=0;i<5;i++){
    fds[i].fd = 0;
     fds[i].events = POLLIN;

  }
   int rc;
  struct sockaddr_in client_addr;
  socklen_t clientSize=sizeof(client_addr);
  int cnt=0;

  if((new_sockfd=accept(sockfd, (struct sockaddr*) &client_addr, &clientSize))==-1)
  {
    perror("accept");
    fds[0].fd=-1;
  }
  fds[0].fd=new_sockfd;


  while(1){

    printf("Waiting on Poll()\n");
    rc = poll(fds, 5, -1);

    if (rc < 0)
    {
      perror("  poll() failed");
      break;
    }else

    {
    for(int i=1;i<2;i++){


        if(fds[i].revents!=0)
        {


          printf("Connection Accepted\n %d",cnt);

          if((new_sockfd=accept(sockfd, (struct sockaddr*) &client_addr, &clientSize))==-1)
          {
            perror("accept");
            //fds[i].fd=-1;
            continue;
          }
          fds[i].fd=new_sockfd;
          //cnt++;



        char buffer[100];
        memset(buffer,0,100);
        int n;
        if((n=read(fds[i].fd, buffer, 99)==-1))
        {

          //fds[i].fd= -1;
          continue;
        }
        printf("The client said: %s\n", buffer);
      }

    }

  }




    printf("Trace 1\n");


  }

return(0);
}
