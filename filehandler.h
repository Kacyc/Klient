#ifndef FILEHANDLER_H
#define FILEHANDLER_H

#include<string>
#include<stdlib.h>
#include<stdio.h>
#include<vector>
#include <sstream>
#include <time.h>
#include <sys/stat.h>
#include <sstream>
class FileHandler
{
private:
  std::string path;
  FILE *fp;
public:
  void setPath(std::string path);
  std::string getPath();
  void ropen();
  void aopen();
  void close();
  std::vector<unsigned char *> getChunks();
  void writeChunks(std::vector<unsigned char *> chunks);
  std::string addzero(int x);
  std::string returndate();
};

#endif