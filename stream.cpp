#include "stream.h"
#include "inotify.h"

Stream::Stream(int filedesc)
{
    this->fd = filedesc;
    this->inotify = NULL;
}

Stream::Stream(int filedesc,Inotify *inotify)
{
    this->fd = filedesc;
    this->inotify = inotify;
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

int Stream::send_data(std::string path, FileHandler* file)
{
    d.pathnamelength = (file->getRelPathName()).length();
    strncpy(d.pathname, (file->getRelPathName()).c_str(), d.pathnamelength);


    d.size = file->getActualFileSize(path);
    d.type = file->getType();

    std::cout << "WDATA: " << std::string(d.pathname) << "  o typie: " << d.type << std::endl;

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

    std::cout << "ODATA: " << d.pathname << "  o typie: " << d.type << std::endl;

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

void Stream::send_file(std::string path, FileHandler* file, Inotify* inotify)
{
    /*
    if(file.path=="GOOD"&&file.name=="BYE")
    {
      send_data(file.path, file.name, filesize, type);
      return;
    }*/

    setFolder(path);

    if (inotify!=NULL)
    {
        file->setInotify(inotify);
    }



    send_data(path,file);	//wyslij nazwe pliku,rozmiar itd..
    if (file->getType()==4)
      return;
    recv_syn();

    file->sendChunks(this);


    std::cout << "Wyslalem plik: " << file->getRelPath() << "/"  << file->getName() << std::endl;


}

FileHandler* Stream::recv_file(std::string path, Inotify* inotify)
{
    last_delete=false;

    FileHandler* file = recv_data();	//odbierz nazwe,rozmiar itd...
    if (file->getType()==4)
    {
      return file;
    }
    if (inotify!=NULL)
    {
        std::cout << "RECV FILE PODLACZAM INOTIFY" << std::endl;
        file->setInotify(inotify);
    }
    /*d.pathname[d.pathnamelength]='\0';
    d.path[d.pathlength]='\0';
    std::string relpath = d.path;
    std::string name = d.name;
    struct path_name recvdfile = {relpath, name};
    if(relpath=="GOOD"&&name=="BYE")
    {
      return recvdfile;
    }
    std::string fullpath = path + "/" + relpath + name;
    */

    send_syn();

    setFolder(path);

    file->processFile(this);


    std::cout << "Odebralem plik: " << file->getName() << "  o rozm.: " << file->getActualFileSize(path)<< std::endl;



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


bool Stream::get_last_delete()
{
    return last_delete;
}

std::string Stream::getFolder()
{
    return folder;
}

void Stream::setFolder(std::string path)
{
    folder = path;
}

