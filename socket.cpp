#include "socket.h"
#include <string.h>
Socket::Socket(std::string addr,int port)
{
  fd = socket(PF_INET,SOCK_STREAM,0);
  sock = new sockaddr_in{AF_INET,htons(port),inet_addr(addr)};sock = new sockaddr_in{AF_INET,htons(port),inet_addr(addr)};
  connect(fd,(sockaddr*) sock,sizeof(sockaddr_in));
}