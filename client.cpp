#include <iostream>
#include "connector.h"
#include "stream.h"

int main(int argc, char **argv) {
    
    //const char* dir;
    //dir = argv[1];
    
    //const char* addr =  argv[2];
    //int port = atoi( argv[3]);
    const char* addr = "127.0.0.1";
    int port = 1330;
  
  
    Connector connector;
    int fd = connector.conn(addr, port);
    
    Stream stream(fd);
    
    int bufsize = 256;
    char buffer[bufsize];
    int br = stream.recv_data(buffer, bufsize);
    
    
    char buffsend[] = {"Czesc "};
    
    stream.send_data(buffsend, sizeof(buffsend)/sizeof(char));
    
    
    
    std::cout << "br: " << br <<std::endl;
    
    return 0;
}
