#include "filehandler.h"


FileHandler::FileHandler(std::string path, int type, int size)
{
  relPathName = path;
  typeOfAction = type;
  sizeOfOriginalFile = size;
  inotify = NULL;
}

FileHandler::~FileHandler()
{
}

int FileHandler::getActualFileSize(std::string path)
{
    std::string fullpath = path + "/" + relPathName;
    struct stat stat_buf;
    int rc = stat(fullpath.c_str(), &stat_buf);
    return rc == 0 ? stat_buf.st_size : -1;
}

void FileHandler::setSizeOfOriginalFile(int size)
{
  sizeOfOriginalFile = size;
}


std::string FileHandler::getRelPathName()
{
  return relPathName;
}


std::string FileHandler::getName()
{
  std::size_t found = relPathName.find_last_of("/\\");
  return relPathName.substr(found+1); 
}

std::string FileHandler::getRelPath()
{
  std::size_t found = relPathName.find_last_of("/\\");
  std::size_t x = -1;
  if(found == x)
    return "";
  else
    return relPathName.substr(0,found); 
}

int FileHandler::getType()
{
  return typeOfAction;
}


void FileHandler::setInotify(Inotify* i)
{
  inotify = i;
}
