#include "connector.h"



int Connector::conn(std::string addr, int port)
{
  if((fd = socket(PF_INET,SOCK_STREAM,0)) == -1 )
  {
      perror ("Can't create socket");
      exit (EXIT_FAILURE);
  }
  
  sock = new sockaddr_in{AF_INET,htons(port),inet_addr(addr.c_str())};
  
  
  
  if(connect(fd,(sockaddr*) sock,sizeof(sockaddr_in)) == -1)
  {
    perror ("Can't connect socket");
    exit (EXIT_FAILURE);
  }
  return fd;
}