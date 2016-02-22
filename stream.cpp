#include "stream.h"


Stream::Stream(int filedesc, std::string path)
{
    fd = filedesc;
    folder = path;
}


int Stream::send_message(char* buffer, int len)
{
    int bytes_sent = send(fd,buffer,len,0);

    return bytes_sent;
}

int Stream::recv_message(char* buffer, int len)
{
    int bytes_recv = recv(fd, buffer, len, 0);

    return bytes_recv;
}

int Stream::send_data(FileHandler* file)
{
    d.pathnamelength = (file->getRelPathName()).length();
    strncpy(d.pathname, (file->getRelPathName()).c_str(), d.pathnamelength);


    d.size = file->getActualFileSize(folder);
    d.type = file->getType();


    if(d.type == 3)
    {
        FileHandlerMove* moved = dynamic_cast<FileHandlerMove*>(file);

        d.prevpathnamelength = (moved->getPrevRelPathName()).length();
        strncpy(d.prevpathname, (moved->getPrevRelPathName()).c_str(), d.prevpathnamelength);
    }

    int bytes_sent = send(fd,&d,sizeof(d),0);

    return bytes_sent;
};

FileHandler* Stream::recv_data()
{
    recv(fd, &d, sizeof(d), 0);
    d.pathname[d.pathnamelength]='\0';

    FileHandler* pFileHandler;


    switch(d.type)
    {
    case 0:
        pFileHandler = new FileHandlerNormal(std::string(d.pathname), d.type, d.size);
        break;
    case 1:
        pFileHandler = new FileHandlerFolder(std::string(d.pathname), d.type, d.size);
        break;
    case 2:
        pFileHandler = new FileHandlerRemove(std::string(d.pathname), d.type, d.size);
        break;
    case 3:
        d.prevpathname[d.prevpathnamelength]='\0';
        pFileHandler = new FileHandlerMove(std::string(d.pathname), d.type,std::string(d.prevpathname) ,d.size);
        break;
    case 4:
	pFileHandler = new FileHandlerNormal(std::string(d.pathname), d.type, d.size);
        break;
    }

    return pFileHandler;
}

void Stream::send_file( FileHandler* file, Inotify* inotify)
{

    if (inotify!=NULL)
    {
        file->setInotify(inotify);
    }

    send_data(file);	//wyslij nazwe pliku,rozmiar itd..
    if (file->getType()==4)	//odlaczanie klienta
      return;
    
    
    recv_syn();

    file->sendChunks(this);	//wyslij zawartosc pliku
    std::cout << "Wyslalem plik: " << file->getRelPathName() << std::endl;


}

FileHandler* Stream::recv_file( Inotify* inotify)
{
    
    FileHandler* file = recv_data();	//odbierz nazwe,rozmiar itd...
    if (file->getType()==4)	//odlaczanie klienta
    {
      return file;
    }
    if (inotify!=NULL)
    {
        
        file->setInotify(inotify);
    }
    

    send_syn();
    file->processFile(this);	//zrealizuj wlasciwe polecenie dla pliku oraz odbierz zawartosc jesli jest
    std::cout << "Odebralem plik: " << file->getRelPathName() << "  o rozm.: " << file->getActualFileSize(folder)<< std::endl;

    return file;
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

void Stream::send_syn()
{
    char buf[4] = "get";
    send_message(buf,4);
}

void Stream::recv_syn()
{
    char buf[4];
    recv_message(buf,4);
}

std::string Stream::getFolder()
{
    return folder;
}

void Stream::setFolder(std::string path)
{
    folder = path;
}

void Stream::sendInitFiles()
{
  
  std::vector<std::string> list = listOfFiles(folder);
  FileHandler* pFileHandler;
  

  for ( auto i = list.begin(); i != list.end(); i++ ) {
	if(isFolder(*i))
	 pFileHandler = new FileHandlerFolder((*i),1);
	else
	 pFileHandler = new FileHandlerNormal((*i),0);
	
	send_file(pFileHandler);
	
	delete pFileHandler;
  }
  
     
}


bool Stream::isFolder(std::string relpathname){
    std::string fullpath = folder + "/" + relpathname;
    struct stat s;
    stat(fullpath.c_str(),&s);
    if( s.st_mode & S_IFDIR )
    {  
      return true;
    }
    else
      return false;
}

std::vector<std::string> Stream::listOfFiles(std::string path) {
  std::vector<std::string> fileList;
  struct dirent *entry;
  DIR *dp;

  dp = opendir(path.c_str());
  if (dp == NULL) {
    perror("opendir");
  }
  
  
  while((entry = readdir(dp)))
  {
    if(entry->d_name[0] == '.')
      continue;
    
    
    fileList.push_back(std::string(entry->d_name));
    
    if(entry->d_type == DT_DIR)
    {
      path += "/"+std::string(entry->d_name);
      std::vector<std::string> tempList = listOfFiles(path);
      for ( auto i = tempList.begin(); i != tempList.end(); i++ ) {
	(*i).insert(0, std::string(entry->d_name)+"/");
      }
      fileList.insert(fileList.end(), tempList.begin(), tempList.end() );
    }
  }
  closedir(dp);
  return fileList;
}