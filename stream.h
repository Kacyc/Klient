#ifndef STREAM_H
#define STREAM_H


#include <sys/socket.h>
#include <netinet/in.h>
//#include <arpa/inet.h>
#include <iostream>
#include <stdio.h>
#include <vector>
#include <string.h>
#include <sys/stat.h>

struct data
{
  char name[20];
  int namesize;
  int size;
};


class Stream
{
private:
    int fd;
    struct data d;
public:
    Stream(int filedesc);
    int send_message(char* buffer, int len);
    int recv_message(char* buffer, int len);
    int send_data(std::string path, std::string filename);
    int recv_data();
    void send_file(std::string path, std::string filename);
    void recv_file(std::string path);
    int get_file_size(std::string filename);
};


#endif