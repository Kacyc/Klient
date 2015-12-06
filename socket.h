#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
class Socket
{
public:
    Socket(std::string addr,int port);
    void connect();
private:
    int fd;
    sockaddr_in* sock;
    std::string addr;
};