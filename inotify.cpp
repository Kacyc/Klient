#include "inotify.h"

Inotify::Inotify(const char* path)
{
  this->path=std::string(path);
  
  this->fd = inotify_init();
  
  if ( fd < 0 ) {
    perror( "inotify_init" );
  }
  
  this->wd = inotify_add_watch( fd, path, IN_CLOSE_WRITE | IN_CREATE |  IN_DELETE | IN_MOVE);

};

Inotify::~Inotify()
{
  ( void ) inotify_rm_watch( fd, wd );

}


std::vector<FileHandler*> Inotify::readNotify()
{
  std::vector<FileHandler*> red;
  char buffer[BUF_LEN];
  int length,i=0;
  
  
  length = read( fd, buffer, BUF_LEN );  
  
  if ( length < 0 ) {
    perror( "read" );
  }  
  
  
  while (i < length) {
    struct inotify_event *event = ( struct inotify_event * ) &buffer[ i ];
    if ( event->len  && event->name[0] != '.' && !shouldIgnore(event->name)) {
    
      std:: string relPath = get_rel_path(event->wd, event->name);
	
      if (event->mask & IN_CLOSE_WRITE  ) {
          printf( "Plik %s zostal stoworzony/zmodyfikowany.\n", event->name );
	  red.push_back(new FileHandlerNormal(relPath+event->name,0));
        
      }
      else if ( event->mask & IN_DELETE ) {
        if ( event->mask & IN_ISDIR ) {
          printf( "Folder %s zostal usuniety.\n", event->name );
	  remove_watch(relPath+event->name);
	  red.push_back(new FileHandlerRemove(relPath+event->name,2));
        }
        else {
          printf( "Plik %s zostal usuniety.\n", event->name );
	  red.push_back(new FileHandlerRemove(relPath+event->name,2));
        }
      }
      else if ( event->mask & IN_CREATE && event->mask & IN_ISDIR ) {
          printf( "Folder %s zostal stworzony.\n", event->name );
	  red.push_back(new FileHandlerFolder(relPath+event->name,1));
	  add_watch(relPath+event->name);
	  
	  std::vector<FileHandler*> tempred = listdir(relPath+event->name);
	  red.insert(red.end(), tempred.begin(), tempred.end() );
        
      }
      else if ( event->mask & IN_MOVED_FROM ) {
         if(event->mask & IN_ISDIR) {
	  printf( "Folder %s zostal przeniesiony z.\n", event->name ); 
	  addCookie(event->cookie, relPath+event->name);
	  remove_watch(relPath+event->name);
	 }
	 else{
	  printf( "Plik %s zostal przeniesiony z.\n", event->name ); 
	  addCookie(event->cookie, relPath+event->name);
	 }
      }
      else if ( event->mask & IN_MOVED_TO ){
        if( event->mask & IN_ISDIR){
	 printf( "Folder %s zostal przeniesiony do.\n", event->name );
	 red.push_back(new FileHandlerMove(relPath+event->name,3,getCookieName(event->cookie)));
	 deleteCookie(event->cookie);
	 add_watch(relPath+event->name);
	 
	}
	else{
	  printf( "Plik %s zostal przeniesiony do.\n", event->name ); 
	  red.push_back(new FileHandlerMove(relPath+event->name,3,getCookieName(event->cookie)));
	  deleteCookie(event->cookie);
	  
	}
      }
    }
    else{
           
      if ((event->mask & IN_CLOSE_WRITE) &&  !(event->mask & IN_ISDIR)  ) {
	removeIgnore(event->name);
      }
      else if((event->mask & IN_CREATE) && (event->mask & IN_ISDIR) ) {
	removeIgnore(event->name);
      }
      else if(event->mask & IN_MOVE) {
	removeIgnore(event->name);
      }
      else if(event->mask & IN_DELETE ) {
	removeIgnore(event->name);
      }
        
    }
      
    i += EVENT_SIZE + event->len;
   
  }
  return red;
  
}

int Inotify::get_fd()
{
  return fd;
}

std::string Inotify::get_path()
{
  return path;
}

std::string Inotify::get_rel_path(int event_wd, std::string event_name)
{
  std::string rel_path; 
  if(event_wd != wd)
  {
	    std::vector<fold_wd>::iterator it = std::find_if(subdirs.begin(), subdirs.end(),  [&](fold_wd& f){ return f.wd == event_wd; } );
	    rel_path = (*it).path + "/";
  }
  else
    rel_path = "";
  
  return rel_path;
}

void Inotify::add_watch(std::string relpathname)
{
  std::string fullpath = std::string(path) + "/" + relpathname;
  int new_wd = inotify_add_watch( fd, fullpath.c_str(), IN_CLOSE_WRITE | IN_CREATE | IN_DELETE |IN_MOVE); 
  std::cout << "Dodaje nowy folder o sciezce absolutnej: " << fullpath << std::endl;
  addSubdir(relpathname, new_wd);
}

void Inotify::remove_watch(std::string relpathname)
{
  int wdToDelete = removeSubdir(relpathname);
  inotify_rm_watch(fd, wdToDelete );
}

void Inotify::addSubdir(std::string rel_path, int wd)
{
  this->subdirs.push_back({rel_path, wd});
}

int Inotify::removeSubdir(std::string path)
{  
  std::vector<fold_wd>::iterator it = std::find_if(subdirs.begin(), subdirs.end(),  [&](fold_wd& f){ return f.path == path; } );
  int wdToDelete = (*it).wd;
  subdirs.erase(it);
  
 
  return wdToDelete;
}



void Inotify::addCookie(int cookie, std::string relPathName)
{
  cookies.push_back({cookie, relPathName});
}

void Inotify::deleteCookie(int cookie)
{
  std::vector<cookie_name>::iterator it = std::find_if(cookies.begin(), cookies.end(),  [&](cookie_name& f){ return f.cookie == cookie; } );
  cookies.erase(it); 
}

std::string Inotify::getCookieName(int cookie)
{
  std::vector<cookie_name>::iterator it = std::find_if(cookies.begin(), cookies.end(),  [&](cookie_name& f){ return f.cookie == cookie; } );
  return (*it).relPathName;
}


void Inotify::addIgnore(std::string path)
{
  ignoreList.push_back(path);
}

void Inotify::removeIgnore(std::string path)
{
  ignoreList.erase(std::remove(ignoreList.begin(), ignoreList.end(), path), ignoreList.end());
}

bool Inotify::shouldIgnore(std::string path)
{
  if(std::find(ignoreList.begin(), ignoreList.end(), path) != ignoreList.end()) 
	return true;
  else return false;
}



std::vector<std::string> Inotify::listrecursive(std::string path) {
  std::vector<std::string> fileList;
  struct dirent *entry;
  DIR *dp;

  dp = opendir(path.c_str());
  if (dp == NULL) {
    perror("opendir");
  }
  
  while((entry = readdir(dp)))
  {
    if(entry->d_name[0] == '.' )
      continue;
    
    
    
    fileList.push_back(entry->d_name);
    
    if(entry->d_type == DT_DIR)
    {
      path += "/"+std::string(entry->d_name);
      std::vector<std::string> tempList = listrecursive(path);
      for ( auto i = tempList.begin(); i != tempList.end(); i++ ) {
	(*i).insert(0, std::string(entry->d_name)+"/");
      }
      fileList.insert(fileList.end(), tempList.begin(), tempList.end() );
    }
  }
  closedir(dp);
  return fileList;
  
}

std::vector<FileHandler*> Inotify::listdir(std::string folder)
{
  std::string fullpath = path + "/" + folder;
  std::vector<std::string> list = listrecursive(fullpath);
  std::vector<FileHandler*> listOfFileHandlers;

      
  for ( auto i = list.begin(); i != list.end(); i++ ) {
    (*i).insert(0, folder+"/");
   
    fullpath = path+"/"+(*i); 
    
      if( !fileExists(fullpath) )
      {  
	
	(*i) = findrelpath(*i);
	fullpath = path+"/"+(*i); 
      }
    
    if(isFolder(fullpath) )
    {
      listOfFileHandlers.push_back(new FileHandlerFolder((*i),1));
      add_watch(*i);
    }
    else
    {
      listOfFileHandlers.push_back(new FileHandlerNormal((*i),0));
    }
  }
      
  return listOfFileHandlers;
}

bool Inotify::fileExists(std::string fullpath)
{
    struct stat stat_buf;
    int rc = stat(fullpath.c_str(), &stat_buf);
    return rc == 0 ? true : false;
}

std::string Inotify::findfullpath(std::string path,std::string relpathname, bool& found)
{
  struct dirent *entry;
  DIR *dp;
  std::string result;
  
  dp = opendir(path.c_str());
  if (dp == NULL) {
    perror("opendir");
  }
  
  while((entry = readdir(dp)))
  {
    if(found)
      break;
    
    if(entry->d_name[0] == '.')
      continue;
    
    std::string newpath = path+"/"+std::string(entry->d_name)+"/"+relpathname;
    if(fileExists(newpath))
    {  
      found = true;
      return newpath;
    }
    else if(entry->d_type == DT_DIR)
    {
      result = findfullpath(path+"/"+std::string(entry->d_name),relpathname,found);
      
    }
    
  }
  return result;
}

std::string Inotify::findrelpath(std::string relpathname)
{
  bool found = false;
  std::string newfullpath = findfullpath(path,relpathname,found);

  int pathlen = path.length();
  std::string newrelpathname = newfullpath.substr(pathlen+1);
  
  
  return newrelpathname;
}

bool Inotify::isFolder(std::string fullpath)
{
    struct stat s;
    stat(fullpath.c_str(),&s);
    if( s.st_mode & S_IFDIR )
    {  
      return true;
    }
    else
      return false;
}
