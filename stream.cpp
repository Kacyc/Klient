#include "stream.h"


Stream::Stream(int filedesc)
{
  fd = filedesc;
}

int Stream::send_data(char* buffer, int len)
{
  int bytes_sent = send(fd,buffer,len,0);
  
  return bytes_sent;
}

int Stream::recv_data(char* buffer, int len)
{
  int bytes_recv = recv(fd, buffer, len, 0);
  buffer[bytes_recv] = '\0';
  std::cout << buffer << std::endl;
  return bytes_recv;
}
