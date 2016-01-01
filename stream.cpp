#include "stream.h"


Stream::Stream(int filedesc)
{
  fd = filedesc;
}

int Stream::send_message(char* buffer, int len)
{
  int bytes_sent = send(fd,buffer,len,0);
  
  return bytes_sent;
}

int Stream::recv_message(char* buffer, int len)
{ 
  int bytes_recv = recv(fd, buffer, len, 0);
  
  buffer[bytes_recv] = '\0';
  std::cout << buffer << std::endl;
  return bytes_recv;
}

int Stream::send_data(std::string path, std::string filename)
{
  std::string fullpath = path + "/" + std::string(filename);
  char name[20];
  strncpy(d.name, filename.c_str(), filename.length() );
  
  d.namesize = filename.length();
  d.name[d.namesize] = '\0';
  d.size = get_file_size(fullpath);
  std::cout << "File: " << d.name << " has: " << d.size << " bytes.  " << d.namesize << std::endl;
  int bytes_sent = send(fd,&d,sizeof(d),0);
  
  return bytes_sent;
};

int Stream::recv_data()
{
  int bytes_recv = recv(fd, &d, sizeof(d), 0);
  
  //std::cout << "File: " << d.name << " has: " << d.size << " bytes.  " << d.namesize << std::endl;
  //name = d.name;
  //filesize = d.size;
  return bytes_recv;
}

void Stream::send_file(std::string path, std::string filename)
{
  std::string fullpath = path + "/" + std::string(filename); 
  int filesize = get_file_size(fullpath);
  char name[20];
  strncpy(name, filename.c_str(), filename.length() );
  
  
 
  send_data(path, filename);
  //send_message(d, sizeof(d));
  
  /*
  std::cout << "sizeof(filename): " << filename << std::endl;
  std::cout << "sizeof(a.txt: " << "a.txt" << std::endl;
  
  std::cout << "sizeof(filename): " << sizeof(filename) << std::endl;
  std::cout << "sizeof(a.txt: " << sizeof("a.txt") << std::endl;
  */
  char buf[4];
  recv_message(buf,4);
  
  if(filesize > 0)
  {
  //fullpath = "/home/mati/test/a.txt";
  //std::cout << fullpath << std::endl;
  FILE *pFile = fopen(fullpath.c_str(), "rb");
  
  int nread;
  char buff[256] = {0};
  do{
    nread = fread(buff,1,256,pFile);
    std::cout << "Bytes read: " << nread << std::endl;
    int bytes_sent = send_message(buff,nread);
    std::cout << "Bytes sent: " << bytes_sent << std::endl;
    
    /*if (nread < 256)
    {
       if (feof(pFile))
	printf("End of file\n");
       if (ferror(pFile))
         printf("Error reading\n");
       
     }
    */
    
  }while(nread > 0);
  fclose (pFile);  
  }
  std::cout << "sent whole file" << std::endl;
  
}

std::string Stream::recv_file(std::string path)
{
  
  
  int bytes = recv_data();
  d.name[d.namesize]='\0';
  
  //int bytes = recv_message(name,20);
  std::cout << "RECFile: " << d.name << " has: " << d.size << " bytes."<< std::endl;
  
  std::string fullpath = path + "/" + std::string(d.name);
  //std::string fullpath = "/home/mati/sv/a.txt";
  //std::cout << fullpath << std::endl;
  
  char buf[4] = "get";
  send_message(buf,4);
  
  FILE *pFile = fopen(fullpath.c_str(), "wb");
  if(d.size > 0)
  {
    
    std::cout << "file created" << std::endl;
    char recvbuf[256] = {0};
    int bytesReceived=0;
    std::cout << "znak" << std::endl;
  
  while((bytesReceived = recv_message(recvbuf,256)) >= 0)
  {
    std::cout << "bytesReceived: " << bytesReceived << std::endl;
    std::cout << "recv_message: " << recvbuf << std::endl;
    std::cout << "sizeof: " << strlen(recvbuf) << std::endl;
    fwrite(recvbuf, 1,bytesReceived,pFile);
    
    if(bytesReceived < 256)
      break;
    
  }
  }
  std::cout << "file writed" << std::endl;
  fclose (pFile);
  
  std::cout << "Jaki plik udalo sie odebrac: " << std::string(d.name) << std::endl;
  return std::string(d.name);
}

int Stream::get_file_size(std::string filename)
{
    struct stat stat_buf;
    int rc = stat(filename.c_str(), &stat_buf);
    return rc == 0 ? stat_buf.st_size : -1;
}

int Stream::get_fd()
{
  return fd;
}
