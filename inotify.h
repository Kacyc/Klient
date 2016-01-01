#ifndef INOTIFY_H
#define INOTIFY_H

#include <iostream>
#include <stdio.h>
#include <time.h>
#include <sys/stat.h>
#include <sstream>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/inotify.h>
#include <string>
#include <unistd.h>
#include <vector>
#define EVENT_SIZE  ( sizeof (struct inotify_event) )
#define BUF_LEN     ( 1024 * ( EVENT_SIZE + 16 ) )
class Inotify
{
private:
  const char* path;
  int fd;
  int wd;
public:
  Inotify(const char* path);
  ~Inotify();
  std::vector<std::string> readNotify();
  std::string addzero(int x);
  std::string returndate(const char* path);
  std::string get_path();
  void add_watch();
  void remove_watch();
  int get_fd();
};


#endif