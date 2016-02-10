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
  int namelength;
  char path[20];
  int pathlength;
  int size;
  int type; //0-file, 1-direcotry
};


class Stream
{
private:
    int fd;
    struct data d;
    Inotify* inotify;
    bool last_delete;
public:
    Stream(int filedesc);
    Stream(int filedesc,Inotify *inotify);
    int send_message(char* buffer, int len);
    int recv_message(char* buffer, int len);
    int send_data(std::string path, std::string filename,int filesize, int type);
    void send_syn();
    void recv_syn();
    int recv_data();
    void send_file(std::string path, path_name file);
    path_name recv_file(std::string path);
    int get_file_size(std::string filename);
    std::string append_part(int size);
    int get_fd();
    bool get_last_delete();
};


#endif