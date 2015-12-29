#include "acceptor.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

Acceptor::Acceptor(std::string my_address, int my_port)
{
  addr = my_address;
  port = my_port;
}


void Acceptor::start()
{
    if((lsfd = socket(PF_INET, SOCK_STREAM, 0)) == -1)
    {
      perror ("Can't create socket");
      exit (EXIT_FAILURE);
    }
    //sock = new sockaddr_in{AF_INET,htons(port),INADDR_ANY};
    sock = {AF_INET,htons(port),INADDR_ANY};
    
    int reuse = 1;
    if (setsockopt(lsfd, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse)) < 0)
        perror("setsockopt(SO_REUSEADDR) failed");
    
     if (setsockopt(lsfd, SOL_SOCKET, SO_REUSEPORT, (const char*)&reuse, sizeof(reuse)) < 0) 
        perror("setsockopt(SO_REUSEPORT) failed");
    
    
    int nbind = bind(lsfd, (sockaddr*) &sock, sizeof(sockaddr_in));
    if(nbind == -1)
    {
      perror ("Can't bind fd to address");
      exit (EXIT_FAILURE);
    }
    
    int nlisten = listen(lsfd, 3);
    if(nlisten == -1)
    {
      perror ("Can't set queue size");
      exit (EXIT_FAILURE);
    }
    
}

int Acceptor::accp()
{
    
    sockaddr_in client_struct;
    socklen_t sizeofclient_struct;
  
    
    fd = accept(lsfd, (sockaddr*) &client_struct, &sizeofclient_struct);
    
    return fd;
}

int Acceptor::get_fd()
{
  return lsfd;
}

Acceptor::~Acceptor()
{
    close(lsfd);
    close(fd);
}