#include "filehandlerfolder.h"

FileHandlerFolder::FileHandlerFolder(std::string path, int type, int size) : FileHandler(path,type,size)
{
}

FileHandlerFolder::~FileHandlerFolder()
{
}

void FileHandlerFolder::sendChunks(Stream* stream)
{
}

void FileHandlerFolder::processFile(Stream* stream)
{
  if(inotify != NULL)
    inotify->addIgnore(getName());
  
  std::string fullpath = stream->getFolder()+"/" +relPathName;
  mkdir(fullpath.c_str(),0777);
  if (inotify != NULL)
  {
    if(inotify->isFolder(relPathName))
      inotify->add_watch(relPathName);    
  }
  
}