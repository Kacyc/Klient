#ifndef INOTIFY_H
#define INOTIFY_H
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/inotify.h>
#include <string>

#define EVENT_SIZE  ( sizeof (struct inotify_event) )
#define BUF_LEN     ( 1024 * ( EVENT_SIZE + 16 ) )

class Inotify
{
private:
  std::string path;
  int fd;
  int wd;
public:
  Inotify(std::string path);
  /*std::string*/void read();
};


#endif