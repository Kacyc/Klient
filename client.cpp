#include <iostream>
#include "connector.h"
#include "stream.h"
#include "inotify.h";
int main(int argc, char **argv) {
    
    const char* dir;
    //dir = argv[1];
    dir="/home/kacyc";
    //const char* addr =  argv[2];
    //int port = atoi( argv[3]);
    const char* addr = "127.0.0.1";
    int port = 1330;
    
    Inotify inotify(dir);
    inotify.readNotify();
  
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
