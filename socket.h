#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>

class Socket
{
public:
    Socket(std::string addr, int port);
private:
    int fd;
    sockaddr_in* sock;
    std::string addr;
};