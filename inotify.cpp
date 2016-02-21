#include "inotify.h"

Inotify::Inotify(const char* path)
{
  this->path=std::string(path);
  
  this->fd = inotify_init();
  
  if ( fd < 0 ) {
    perror( "inotify_init" );
  }
  
  this->wd = inotify_add_watch( fd, path, IN_CLOSE_WRITE | IN_CREATE |  IN_DELETE | IN_MOVE);

  this->isFolderToRename = false;
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
  std::string line;
  
  
  length = read( fd, buffer, BUF_LEN );  
  
  if ( length < 0 ) {
    perror( "read" );
  }  
  
  
  while (i < length) {
    struct inotify_event *event = ( struct inotify_event * ) &buffer[ i ];
    if ( event->len  && event->name[0] != '.' && !shouldIgnore(event->name)) {
    std::cout << "INOTIFY EVENT NAME: " << event->name << std::endl;
      std:: string relPath = get_rel_path(event->wd, event->name);
	std::cout << "NIE MA BLEDU: " << event->name << std::endl;
      if (event->mask & IN_CLOSE_WRITE  ) {
          printf( "The file %s was created/modified.\n", event->name );
	  red.push_back(new FileHandlerNormal(relPath+event->name,0));
        
      }
      else if ( event->mask & IN_DELETE ) {
        if ( event->mask & IN_ISDIR ) {
          printf( "The directory %s was deleted.\n", event->name );
	  remove_watch(relPath+event->name);
	  red.push_back(new FileHandlerRemove(relPath+event->name,2));
        }
        else {
          printf( "The file %s was deleted.\n", event->name );
	  red.push_back(new FileHandlerRemove(relPath+event->name,2));
        }
      }
      else if ( event->mask & IN_CREATE && event->mask & IN_ISDIR ) {
          printf( "The directory %s was created.\n", event->name );
	  red.push_back(new FileHandlerFolder(relPath+event->name,1));
	  add_watch(relPath+event->name);
	  
	  std::vector<FileHandler*> tempred = listdir(relPath+event->name);
	  red.insert(red.end(), tempred.begin(), tempred.end() );
        
      }
      else if ( event->mask & IN_MOVED_FROM ) {
         if(event->mask & IN_ISDIR)
	 {
	  printf( "The directory %s was moved from.\n", event->name ); 
	  addCookie(event->cookie, relPath+event->name);
	  remove_watch(relPath+event->name);
	 }
	 else
	 {
	  printf( "The file %s was moved from.\n", event->name ); 
	  addCookie(event->cookie, relPath+event->name);
	  
	 }
      }
      else if ( event->mask & IN_MOVED_TO ){
        if( event->mask & IN_ISDIR)
	{
	 printf( "The directory %s was moved to.\n", event->name );
	 red.push_back(new FileHandlerMove(relPath+event->name,3,getCookieName(event->cookie)));
	 deleteCookie(event->cookie);
	 add_watch(relPath+event->name);
	 
	}
	else
	{
	  printf( "The file %s was moved to.\n", event->name ); 
	  red.push_back(new FileHandlerMove(relPath+event->name,3,getCookieName(event->cookie)));
	  deleteCookie(event->cookie);
	  
	}
      }
    }
    else
    {
     
      //std::cout << "file ignored: " << event->name << std::endl;
      
      if ((event->mask & IN_CLOSE_WRITE) &&  !(event->mask & IN_ISDIR)  ) {
	//std::cout << "CLOSEWRITE: " << std::endl;
	
	removeIgnore(event->name);
	
      }
      else if((event->mask & IN_CREATE) && (event->mask & IN_ISDIR) ) 
      {
	//std::cout << "CRAETE: " << std::endl;
	removeIgnore(event->name);
      }
      else if(event->mask & IN_MOVE) {
	//std::cout << "MOVE: " << std::endl;
	removeIgnore(event->name);
	
      }
      else if(event->mask & IN_DELETE ) {
	//std::cout << "DELETE" << std::endl;
	removeIgnore(event->name);
      }
        
      
      
    }
      
    i += EVENT_SIZE + event->len;
   
  }
  return red;
  
}




std::string Inotify::addzero(int x)
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

std::string Inotify::returndate(const char* path)
{
  struct tm* clock;               
  struct stat attrib;         
  stat(path, &attrib);
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

std::string Inotify::get_path()
{
    return path;
}

int Inotify::get_fd()
{
    return fd;
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
 /* std::cout << "-----------1SUBDIRS-------" << std::endl;
  for ( auto i = subdirs.begin(); i != subdirs.end(); i++ ) {
    std::cout << (*i).path  << std::endl;
    
  }
  std::cout << "------------------------" << std::endl;
  */
  
  
  std::vector<fold_wd>::iterator it = std::find_if(subdirs.begin(), subdirs.end(),  [&](fold_wd& f){ return f.path == path; } );
  int wdToDelete = (*it).wd;
  subdirs.erase(it);
  
 /* std::cout << "-----------2SUBDIRS-------" << std::endl;
  for ( auto i = subdirs.begin(); i != subdirs.end(); i++ ) {
    std::cout << (*i).path  << std::endl;
    
  }
  std::cout << "------------------------" << std::endl;
  */
 
  return wdToDelete;
}

bool Inotify::has_suffix(const std::string &str, const std::string &suffix)
{
    return str.size() >= suffix.size() &&
           str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
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
  //std::cout << "Ignoruje plik o nazwie: " << path << std::endl;
  ignoreList.push_back(path);
 
  /*
  std::cout << "------------- Ignorowane pliki-------------" << std::endl;
  for(std::vector<std::string>::iterator it = ignoreList.begin(); it != ignoreList.end(); ++it) 
    std::cout << *it << std::endl;
    
  std::cout << "-----------------------------------------------" << std::endl;
*/
}

void Inotify::removeIgnore(std::string path)
{
  ignoreList.erase(std::remove(ignoreList.begin(), ignoreList.end(), path), ignoreList.end());
  //std::cout << "Przestaje ignorowac plik o nazwie: " << path << std::endl;
  
  /*  std::cout << "------------- Ignorowane pliki-------------" << std::endl;
  for(std::vector<std::string>::iterator it = ignoreList.begin(); it != ignoreList.end(); ++it) 
    std::cout << *it << std::endl;
    
    std::cout << "-----------------------------------------------" << std::endl;

  */
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
    
    
    //std::cout << entry->d_name << std::endl;
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
  /*
  for ( auto i = list.begin(); i != list.end(); i++ ) {
	(*i).insert(0, folder+"/");
  }
*/
      
  for ( auto i = list.begin(); i != list.end(); i++ ) {
    (*i).insert(0, folder+"/");
   
    fullpath = path+"/"+(*i); 
    
      if( !fileExists(fullpath) )
      {  
	//std::cout << "PLIK NIE ISTNIEJE: " << fullpath << std::endl;
	(*i) = findrelpath(*i);
	//std::cout << "ZAMIANA Z: " << (*i).path << " na: " << newrelpath.path << std::endl;
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

bool Inotify::fileExists(std::string filename)
{
    struct stat stat_buf;
    int rc = stat(filename.c_str(), &stat_buf);
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
  //std::cout << "11FILENAME.PATH: " << filename.path << "  FILENAME.NAME: " << filename.name << std::endl;
  std::string newfullpath = findfullpath(path,relpathname,found);

  int pathlen = path.length();
  //std::cout << "FULLPATH: " << fullpath << std::endl;
  //std::cout << "FILENAME.PATH: " << filename.path << "  FILENAME.NAME: " << filename.name << std::endl;
  std::string newrelpathname = newfullpath.substr(pathlen+1);
  
  
  return newrelpathname;
}

bool Inotify::isFolder(std::string relpathname)
{
    std::string fullpath = path + "/" + relpathname;
    struct stat s;
    stat(fullpath.c_str(),&s);
    if( s.st_mode & S_IFDIR )
    {  
      return true;
    }
    else
      return false;
}
