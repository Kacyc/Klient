#ifndef CONNECTOR_H
#define CONNECTOR_H

//#include <fcntl.h>
//#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <iostream>

class Connector
{
private:
    int fd;
    sockaddr_in* sock;
    std::string addr;  
public:
    int conn(std::string addr, int port);

};


#endif