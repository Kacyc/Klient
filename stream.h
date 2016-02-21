#ifndef STREAM_H
#define STREAM_H

class Inotify;

#include <sys/socket.h>
#include <netinet/in.h>
//#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <stdio.h>
#include <vector>
#include <string.h>
#include <sys/stat.h>


#include "filehandlernormal.h"
#include "filehandlerremove.h"
#include "filehandlerfolder.h"
#include "filehandlermove.h"

struct data
{
  char pathname[30];
  int pathnamelength;
  int size;
  int type; 
  char prevpathname[30];
  int prevpathnamelength;
};


class Stream
{
private:
    int fd;
    struct data d;
    Inotify* inotify;
    bool last_delete;
    std::string folder;
public:
    Stream(int filedesc);
    Stream(int filedesc,Inotify *inotify);
    int send_message(char* buffer, int len);
    int recv_message(char* buffer, int len);
    int send_data(std::string path, FileHandler* file);
    void send_syn();
    void recv_syn();
    FileHandler* recv_data();
    void send_file(std::string path, FileHandler* file, Inotify* inotify = NULL);
    FileHandler* recv_file(std::string path,Inotify* inotify = NULL);
    int get_file_size(std::string filename);
    std::string append_part(int size);
    int get_fd();
    void setFolder(std::string path);
    std::string getFolder();
    bool get_last_delete();
};


#endif