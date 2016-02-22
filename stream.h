#ifndef STREAM_H
#define STREAM_H

class Inotify;

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
#include <stdio.h>
#include <vector>
#include <string.h>
#include <sys/stat.h>


#include "filehandlernormal.h"
#include "filehandlerremove.h"
#include "filehandlerfolder.h"
#include "filehandlermove.h"

struct data
{
  char pathname[30];
  int pathnamelength;
  int size;
  int type; 
  char prevpathname[30];
  int prevpathnamelength;
};



class Stream
{
private:
    int fd;
    struct data d;	//struktura do przesylania przez gniazdo
    std::string folder;
public:
    Stream(int filedesc, std::string path="");
    int send_message(char* buffer, int len);	//wysylanie przez gniazdo bufora - buffer
    int recv_message(char* buffer, int len);
    int send_data(FileHandler* file);	//wysylanie inoframcji o nazwie pliku, rozmiarze itd.
    FileHandler* recv_data();
    void send_syn();	//wysylanie wiadomosci synchronizujacej
    void recv_syn();
    void send_file(FileHandler* file, Inotify* inotify = NULL);	//wysylanie pliku
    FileHandler* recv_file(Inotify* inotify = NULL);
    int get_file_size(std::string filename);
    int get_fd();
    void setFolder(std::string path);
    bool isFolder(std::string relpathname);
    std::string getFolder();
    
    void sendInitFiles();	//poczatkowe wysylanie plikow posiadanych przez serwer do nowopodlaczonego klienta
    std::vector<std::string> listOfFiles(std::string path);	//zwraca nazwy plikow w folderze serwera

};
#endif