#ifndef ACCEPTOR_H
#define ACCEPTOR_H

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>

class Acceptor
{
private:
    int lsfd;
    int fd;
    sockaddr_in sock;
    std::string addr;
    int port;
public:
    Acceptor(std::string my_address, int my_port);
    void start();
    int get_fd();
    int accp();
    ~Acceptor();
};


#endif