#ifndef STREAM_H
#define STREAM_H


#include <sys/socket.h>
#include <netinet/in.h>
//#include <arpa/inet.h>
#include <iostream>

class Stream
{
private:
    int fd;
    sockaddr_in* sock;
    std::string addr;
public:
    Stream(int filedesc);
    int send_data(char* buffer, int len);
    int recv_data(char* buffer, int len);
};


#endif