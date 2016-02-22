#include "filehandlerremove.h"

FileHandlerRemove::FileHandlerRemove(std::string path, int type, int size) : FileHandler(path,type,size)
{ 
}

FileHandlerRemove::~FileHandlerRemove()
{
}

void FileHandlerRemove::sendChunks(Stream* stream)
{
}

void FileHandlerRemove::processFile(Stream* stream)
{
  if(inotify != NULL)
  {
    inotify->addIgnore(getName());
  }
  std::string fullpath = stream->getFolder()+"/" +relPathName;
  remove(fullpath.c_str());
  
  if(inotify != NULL )
  {
    if(inotify->isFolder(fullpath))
      inotify->remove_watch(relPathName);    
  }
}