#include "stream.h"


Stream::Stream(int filedesc)
{
  fd = filedesc;
}

int Stream::send_message(char* buffer, int len)
{
  int bytes_sent = send(fd,buffer,len,0);
  
  return bytes_sent;
}

int Stream::recv_message(char* buffer, int len)
{
  int bytes_recv = recv(fd, buffer, len, 0);
  buffer[bytes_recv] = '\0';
  std::cout << buffer << std::endl;
  return bytes_recv;
}

int Stream::send_data(int type, std::vector <std::string> names, std::vector <std::string> dates)
{
  if(type = 1)
  {
    d->type = type;
    d->names = names;
    d->dates = dates;
  }
  
  int bytes_sent = send(fd,d,sizeof(d),0);
  return bytes_sent;
  
};

int Stream::recv_data(std::vector <std::string> &names, std::vector <std::string> &dates)
{
  struct data d;
  int size = 256;
  int bytes_recv = recv(fd,(void*) &d, size, 0);
  
  names = d.names;
  dates = d.dates;
}