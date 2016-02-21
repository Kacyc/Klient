#ifndef FILEHANDLERMOVE_H
#define FILEHANDLERMOVE_H



#include "filehandler.h"
#include "inotify.h"

class FileHandlerMove : public FileHandler{
private:
  std::string prevRelPathName;
public:
  FileHandlerMove(std::string path, int type, std::string prevpath, int size=0);
  virtual ~FileHandlerMove();
  virtual void sendChunks(Stream* stream);
  virtual void processFile(Stream* stream);
  std::string getPrevRelPathName();
  std::string getPrevName();
};

#endif