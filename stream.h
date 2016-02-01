#ifndef STREAM_H
#define STREAM_H


#include <sys/socket.h>
#include <netinet/in.h>
//#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <stdio.h>
#include <vector>
#include <string.h>
#include <sys/stat.h>
#include "inotify.h"

struct data
{
  char name[20];
  int namesize;
  int size;
  int type; //0-file, 1-direcotry
};


class Stream
{
private:
    int fd;
    struct data d;
    Inotify* inotify;
public:
    Stream(int filedesc);
    Stream(int filedesc,Inotify *inotify);
    int send_message(char* buffer, int len);
    int recv_message(char* buffer, int len);
    int send_data(std::string path, std::string filename,int filesize, int type);
    void send_syn();
    void recv_syn();
    int recv_data();
    void send_file(std::string path, std::string filename );
    std::string recv_file(std::string path);
    int get_file_size(std::string filename);
    std::string append_part(int size);
    int get_fd();
};


#endif