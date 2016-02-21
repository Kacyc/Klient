#ifndef FILEHANDLER_H
#define FILEHANDLER_H

class Stream;
class Inotify;

#include<string>
#include<stdlib.h>
#include<stdio.h>
#include<vector>
#include <sstream>
#include <time.h>
#include <sys/stat.h>



class FileHandler
{
protected:
  std::string relPathName;
  int sizeOfOriginalFile;
  int typeOfAction; //0-normal,1-folder,2-remove,3-move
  Inotify* inotify;
public:
  FileHandler(std::string path, int type, int size);
  virtual ~FileHandler();
  virtual void sendChunks(Stream* stream) = 0;
  virtual void processFile(Stream* stream) = 0;
  int getActualFileSize(std::string path);
  void setSizeOfOriginalFile(int size);
  std::string getRelPathName();
  std::string getName();
  std::string getRelPath();
  int getType();
  void setInotify(Inotify* i);
};

#endif