#include "filehandlernormal.h"

FileHandlerNormal::FileHandlerNormal(std::string path, int type, int size):FileHandler(path,type,size)
{
}

FileHandlerNormal::~FileHandlerNormal()
{
}


void FileHandlerNormal::sendChunks(Stream* stream)
{
  std::string fullpath = stream->getFolder() + "/" + getRelPathName();
  int size_to_send;
  
  if((size_to_send = getActualFileSize(stream->getFolder())) > 0 )
  {
 
  FILE *pFile = fopen(fullpath.c_str(), "rb");	//otworz plik, czytaj z niego, i wysylaj
  
  int nread;
  char buff[256] = {0};
  while(size_to_send > 0){
    
    
    nread = fread(buff,1,256,pFile); 
    int bytes_sent = stream->send_message(buff,nread);
    size_to_send -= bytes_sent;
   
    
    stream->recv_syn();
    
  }
  fclose (pFile);  
  }
}


void FileHandlerNormal::processFile(Stream* stream)
{
  std::string fullpath = stream->getFolder()+"/" +relPathName;
  
  if(inotify != NULL)
   inotify->addIgnore(getName());
  
  FILE *pFile = fopen(fullpath.c_str(), "wb");	//otworz plik o podanej nazwie i zapisuj do niego
  int filesize;
  
    if((filesize=sizeOfOriginalFile) > 0)
    {
     
      char recvbuf[256] = {0};
      int bytesReceived=0;
      
  
      while(filesize > 0)
      {
	
	bytesReceived = stream->recv_message(recvbuf,256); 
	int bytes_sent = fwrite(recvbuf, 1,bytesReceived,pFile);
	filesize -= bytes_sent;
	
	
	stream->send_syn();
      }
    }
    
    fclose(pFile);
}

