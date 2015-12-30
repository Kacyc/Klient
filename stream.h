#ifndef STREAM_H
#define STREAM_H


#include <sys/socket.h>
#include <netinet/in.h>
//#include <arpa/inet.h>
#include <iostream>
#include <vector>

struct data
{
  int type;
  std::vector <std::string> names;
  std::vector <std::string> dates;
};


class Stream
{
private:
    int fd;
    struct data *d;
public:
    Stream(int filedesc);
    int send_message(char* buffer, int len);
    int recv_message(char* buffer, int len);
    int send_data(int type, std::vector<std::string> names, std::vector <std::string> dates);
    int recv_data(std::vector<std::string> &names, std::vector <std::string> &dates);
  
};


#endif