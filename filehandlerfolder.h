#ifndef FILEHANDLERFOLDER_H
#define FILEHANDLERFOLDER_H



#include "filehandler.h"
#include "inotify.h"

class FileHandlerFolder : public FileHandler{
public:
  FileHandlerFolder(std::string path, int type, int size=0);
  virtual ~FileHandlerFolder();
  virtual void sendChunks(Stream* stream);
  virtual void processFile(Stream* stream);  
};

#endif