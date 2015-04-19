/*
    @author : Ganesh Joshi
    @class  : CSCI 611
    @Project: 4 Server with multiple client
*/
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
  int rc;
 struct sockaddr_in client_addr;
 socklen_t clientSize=sizeof(client_addr);
 int cnt=1;

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
    fds[i].fd=-1;
    fds[i].events=POLLIN;
  }


  fds[0].fd = sockfd;
  fds[0].events = POLLIN;
  int current_size;
  while(1)
{


  printf("Waiting on Poll()\n");
  rc = poll(fds, 5, -1);

    //readData(fds);


    printf("Waiting for new connection!!\n");
    //accept new connection

    current_size = cnt;

    for (int i = 0; i < current_size; i++)
    {

            if(fds[i].revents == 0)
              continue;


            if (fds[i].fd == sockfd)
            {
              if(current_size==5)
                close(accept(sockfd, (struct sockaddr*) &client_addr, &clientSize));
              //printf("Value of RC is %d",rc);
              if((fds[cnt].fd=accept(sockfd, (struct sockaddr*) &client_addr, &clientSize))==-1)
              {
                perror("accept");

              }

              //printf("New connection Accepted!!%d\n",fds[cnt].fd);
              fds[cnt].events = POLLIN;
              cnt++;
            }
            else{
              //printf("Reading out\n");
              //readData(fds);
              char buffer[100];
              memset(buffer,0,100);
              int n;

              for(int i=1;i<5;i++)
              {


                  if(fds[i].revents!=0)
                  {
                  if((n=read(fds[i].fd, buffer, 99)==-1))
                  {

                    printf("Issue in read\n");

                  }

                  /*if(n==0){
                    fds[i].fd=-1;
                  }*/

                  printf("The client %d said: %s\n", i,buffer);
                }
              }


            }

    }
  //  printf("New connection Accepted!!%d\n",);

}


}//end of main
