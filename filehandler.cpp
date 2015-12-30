#include "filehandler.h"

void FileHandler::setPath(std::string path)
{
  this->path=path;
}

std::string FileHandler::getPath()
{
  return path;
}

void FileHandler::ropen()
{
  fp = fopen(path.c_str(),"rb");
}

void FileHandler::aopen()
{
  fp = fopen(path.c_str(),"ab");
}

void FileHandler::close()
{
  fclose(fp);
}

std::vector<unsigned char *> FileHandler::getChunks()
{
  std::vector< unsigned char *> chunks;
  ropen();
  while(1)
  {
    unsigned char *buff= new unsigned char[256];
    int nread = fread(buff,1,256,fp);
    printf("Bytes read %d \n", nread);
    if(nread > 0)
    {
      chunks.push_back(buff);
    }
    if (nread < 256)
    {
	if (feof(fp))
	  printf("End of file\n");
	if (ferror(fp))
	  printf("Error reading\n");
	break;
    }
  }
  close();
  return chunks;
}

void FileHandler::writeChunks(std::vector<unsigned char *> chunks)
{
  aopen();
  for (int i =0;i<chunks.size();i++)
  {
    fwrite(chunks[i], 1,256,fp);
  }
  close();
}

std::string FileHandler::addzero(int x)
{
  std::ostringstream ss;
  ss << x;
  
  if(x < 10)
  {
      std::string temp = "0";
      
      
      temp += ss.str();
      //std::cout << "addzero: "  << temp << std::endl;
      return temp;
  }
  else
    return ss.str();
  
}

std::string FileHandler::returndate()
{
  struct tm* clock;               
  struct stat attrib;         
  stat(path.c_str(), &attrib);
  clock = gmtime(&(attrib.st_mtime));
  std::string tempdate;
  
  
  tempdate = addzero(clock->tm_year);
  //std::cout << "year: " << tempdate << std::endl;
  tempdate += addzero(clock->tm_mon);
  //std::cout << "monthr: " << tempdate << std::endl;
  tempdate += addzero(clock->tm_mday);
  //std::cout << "day: " << tempdate << std::endl;
  tempdate += addzero(clock->tm_hour);
  //std::cout << "hour: " << tempdate << std::endl;
  tempdate += addzero(clock->tm_min);
  //std::cout << "min: " << tempdate << std::endl;
  tempdate += addzero(clock->tm_sec);
  //std::cout << "sec: " << tempdate << std::endl;
  
 
  
  return tempdate;
}