#ifndef FILEHANDLERREMOVE_H
#define FILEHANDLERREMOVE_H


#include "filehandler.h"
#include "inotify.h"

class FileHandlerRemove : public FileHandler{
public:
  FileHandlerRemove(std::string path, int type, int size=0);
  virtual ~FileHandlerRemove();
  virtual void sendChunks(Stream* stream);
  virtual void processFile(Stream* stream);
};

#endif