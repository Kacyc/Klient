#ifndef INOTIFY_H
#define INOTIFY_H

#include <iostream>
#include <stdio.h>
#include <dirent.h>
#include <time.h>
#include <sys/stat.h>
#include <sstream>
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

//struktura przechowujaca sciezke wzgledna pliku i jego nazwe
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
  bool isFolderToRename;
  std::string prevFolderName;
  std::vector<std::string> ignoreList;
  std::vector<fold_wd> subdirs;
  std::vector<cookie_name> cookies;
public:
  Inotify(const char* path);
  ~Inotify();
  
  std::vector<FileHandler*> readNotify();
  std::string addzero(int x);
  std::string returndate(const char* path);
  std::string get_path();
  void add_watch(std::string relpathname);
  void remove_watch(std::string relpathname);
  int get_fd();
  std::string get_rel_path(int event_wd,std::string event_name);	//zwraca sciezke wzgledna do pliku o nazwie event_name (bez nazwy)
  bool has_suffix(const std::string &str, const std::string &suffix);
  void addCookie(int cookie, std::string relPathName);
  void deleteCookie(int cookie);
  std::string getCookieName(int cookie);
  void addIgnore(std::string);
  void addSubdir(std::string rel_path, int wd);
  int removeSubdir(std::string path);
  void removeIgnore(std::string);
  bool shouldIgnore(std::string);
  bool fileExists(std::string fullpath);
  bool isFolder(std::string fullpath);
  std::vector<std::string> listrecursive(std::string path);
  std::vector<FileHandler*> listdir(std::string folder);
  std::string findfullpath(std::string path, std::string relpathname, bool& found);
  std::string findrelpath(std::string relpathname);
};


#endif