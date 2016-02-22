#ifndef INOTIFY_H
#define INOTIFY_H

#include <iostream>
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/inotify.h>
#include <string>
#include <unistd.h>
#include <vector>
#include <algorithm>
#include "filehandlernormal.h"
#include "filehandlerremove.h"
#include "filehandlerfolder.h"
#include "filehandlermove.h"

#define EVENT_SIZE  ( sizeof (struct inotify_event) )
#define BUF_LEN     ( 1024 * ( EVENT_SIZE + 16 ) )

//struktura przechowujaca sciezke wzgledna do podfolderu i jego watch deskryptor
struct fold_wd
{
  std::string path;
  int wd;
};

//struktura przechowujaca ciasteczko zdarzenia pliku i sciezke wzgledna pliku 
struct cookie_name
{
    int cookie;
    std::string relPathName;
};

class Inotify
{
private:
  std::string path;
  int fd;
  int wd;
  std::vector<std::string> ignoreList;	//pliki ignorowane
  std::vector<fold_wd> subdirs;	//podfoldery
  std::vector<cookie_name> cookies;	//ciasteczko sluzace do obslugi polecenia mv
public:
  Inotify(const char* path);
  ~Inotify();
  
  std::vector<FileHandler*> readNotify();	//obsluguje zdarzenia od inotify
  int get_fd();
  std::string get_path();
  void add_watch(std::string relpathname);	//dodaje folder do obserwowanych przez inotify
  void remove_watch(std::string relpathname);	
  std::string get_rel_path(int event_wd,std::string event_name);	//zwraca sciezke wzgledna do pliku o nazwie event_name
  void addCookie(int cookie, std::string relPathName);	//dodaje ciasteczko
  void deleteCookie(int cookie);
  std::string getCookieName(int cookie);	//zwraca poprzednia nazwe pliku ze sciezka wzgledna po ciasteczku
  void addIgnore(std::string);	//dodaje plik do ignorowanych
  void removeIgnore(std::string);
  bool shouldIgnore(std::string);
  void addSubdir(std::string rel_path, int wd);	//dodaje podfolder do listy podfolderow dzieki czemu w razie zdarzenia wiemy w ktorym podfolderze nastapilo
  int removeSubdir(std::string path);
  bool fileExists(std::string fullpath);
  bool isFolder(std::string fullpath);
  std::vector<std::string> listrecursive(std::string path);	//wywolywana w listdir - przechodzi po folderze
  std::vector<FileHandler*> listdir(std::string folder);	//zwraca liste plikow w folderze (uzywane gdy wystepuje kopiowanie folderu z plikami w srodku, aby miec pewnosc ze nawet pliki ktorych inotify nie wylapie zostana skopiowane)
  std::string findfullpath(std::string path, std::string relpathname, bool& found);
  std::string findrelpath(std::string relpathname);	//zwraca poprawna sciezke wzgledna do pliku, uzywana gdy 
};


#endif