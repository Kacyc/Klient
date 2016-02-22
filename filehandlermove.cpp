#include "filehandlermove.h"

FileHandlerMove::FileHandlerMove(std::string path, int type, std::string prevpath, int size) : FileHandler(path,type,size)
{
  prevRelPathName = prevpath;
}

FileHandlerMove::~FileHandlerMove()
{
}


void FileHandlerMove::sendChunks(Stream* stream)
{
}

void FileHandlerMove::processFile(Stream* stream)
{
  
  
  std::string fullpath = stream->getFolder()+"/" +relPathName;
  std::string prevfullpath = stream->getFolder()+"/" +prevRelPathName;
  
  if(inotify != NULL)
  {
    inotify->addIgnore(getName());
    inotify->addIgnore(getPrevName());
  }
    
    
  rename(prevfullpath.c_str(), fullpath.c_str());
  
  if(inotify != NULL )
  {
    if(inotify->isFolder(fullpath))
    {
      
      inotify->remove_watch(prevRelPathName);    
      inotify->add_watch(relPathName);
    }
  }

  
}

std::string FileHandlerMove::getPrevRelPathName()
{
  return prevRelPathName;
}


std::string FileHandlerMove::getPrevName()
{
  std::size_t found = prevRelPathName.find_last_of("/\\");
  return prevRelPathName.substr(found+1); 
}