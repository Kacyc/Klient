#include "inotify.h"

Inotify::Inotify(const char* path)
{
  this->path=path;
  
  this->fd = inotify_init();
  
  if ( fd < 0 ) {
    perror( "inotify_init" );
  }
  
  this->wd = inotify_add_watch( fd, path, IN_CLOSE_WRITE | IN_CREATE |  IN_DELETE |IN_MOVE);

};


std::vector<path_name> Inotify::readNotify()
{
  std::vector<path_name> red;
  char buffer[BUF_LEN];
  int length,i=0;
  std::string line;
  struct path_name fileToSend;
  
  length = read( fd, buffer, BUF_LEN );  
  
  if ( length < 0 ) {
    perror( "read" );
  }  
  
  
  while (i < length) {
    struct inotify_event *event = ( struct inotify_event * ) &buffer[ i ];
    if ( event->len  && event->name[0] != '.' && !shouldIgnore(event->name)) {
      fileToSend.name = std::string(event->name);
      fileToSend.path = get_rel_path(event->wd, fileToSend.name);
      
	  
      if (event->mask & IN_CLOSE_WRITE  ) {
        if ( event->mask & IN_ISDIR ) {
          printf( "The directory %s was created/modified.\n", event->name );
        }
        else {
          printf( "The file %s was created/modified.\n", event->name );
	  red.push_back(fileToSend);
        }
      }
      else if ( event->mask & IN_DELETE ) {
        if ( event->mask & IN_ISDIR ) {
          printf( "The directory %s was deleted.\n", event->name );
	  red.push_back(fileToSend);
        }
        else {
          printf( "The file %s was deleted.\n", event->name );
	  red.push_back(fileToSend);
        }
      }
      else if ( event->mask & IN_CREATE && event->mask & IN_ISDIR ) {
        if ( event->mask & IN_ISDIR ) {
          printf( "The directory %s was created.\n", event->name );
	  red.push_back(fileToSend);
	  
	  
	  std::string new_abs_path = std::string(path) + "/" + fileToSend.path + fileToSend.name;
	  std::vector<path_name> tempred = listdir(new_abs_path,fileToSend.name);
	  red.insert(red.end(), tempred.begin(), tempred.end() );
	  
	  int new_wd = inotify_add_watch( fd, new_abs_path.c_str(), IN_CLOSE_WRITE | IN_CREATE | IN_DELETE |IN_MOVE); 
	  std::cout << "Dodaje nowy folder o sciezce absolutnej: " << new_abs_path << std::endl;
	  std::cout << "Dodaje nowy folder o sciezce wzglednej: " << fileToSend.path << fileToSend.name << std::endl;
	  addSubdir(fileToSend.path+fileToSend.name, new_wd);
	  
        }
      }
      else if ( event->mask & IN_MOVED_FROM ) {
         if(!has_suffix(event->name,".part"))
	 {
	  printf( "The file/directory %s was moved from.\n", event->name ); 
	  std::string rel_path = get_rel_path(event->wd, std::string(event->name));
	  red.push_back(fileToSend);
	 }
	 else
	  cookies.push_back(event->cookie);
        
      }
      else if ( event->mask & IN_MOVED_TO && !has_suffix(event->name,".part")){
        if( !is_cookie_on_vector(event->cookie) )
	{
	 printf( "The file/directory %s was moved to.\n", event->name ); 
	 std::string rel_path = get_rel_path(event->wd, std::string(event->name));
	 red.push_back(fileToSend);
	}
	else
	  delete_cookie(event->cookie); 
      }
    }
    else
    {
     
      std::cout << "file ignored: " << event->name << std::endl;
      
      if ((event->mask & IN_CLOSE_WRITE) &&  !(event->mask & IN_ISDIR)  ) {
	std::cout << "CLOSEWRITE: " << std::endl;
	
	removeIgnore(event->name);
	
      }
      else if((event->mask & IN_CREATE) && (event->mask & IN_ISDIR) ) 
      {
	std::cout << "CRAETE: " << std::endl;
	removeIgnore(event->name);
      }
      else if(event->mask & IN_MOVE) {
	std::cout << "MOVE: " << std::endl;
	removeIgnore(event->name);
	
      }
      else if(event->mask & IN_DELETE ) {
	std::cout << "DELETE" << std::endl;
	removeIgnore(event->name);
      }
        
      
      
      
      
      
    }
      
    i += EVENT_SIZE + event->len;
  }
  
  return red;
  
}
Inotify::~Inotify()
{
  ( void ) inotify_rm_watch( fd, wd );

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

void Inotify::add_watch()
{
  this->wd = inotify_add_watch( fd, path, IN_CLOSE_WRITE | IN_CREATE | IN_DELETE | IN_MOVED_TO | IN_MOVED_FROM);
}

void Inotify::remove_watch()
{
  ( void ) inotify_rm_watch( fd, wd );
}

bool Inotify::has_suffix(const std::string &str, const std::string &suffix)
{
    return str.size() >= suffix.size() &&
           str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

bool Inotify::is_cookie_on_vector(int cookie)
{
  std::vector<int>::iterator it = std::find(cookies.begin(), cookies.end(), cookie);
  if(it != cookies.end())
    return true;
  else
    return false;
}

void Inotify::delete_cookie(int cookie)
{
  std::vector<int>::iterator it = std::find(cookies.begin(), cookies.end(), cookie);
  cookies.erase(it); 
}

void Inotify::addIgnore(std::string path)
{
  std::cout << "Ignoruje plik o nazwie: " << path << std::endl;
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
  std::cout << "Przestaje ignorowac plik o nazwie: " << path << std::endl;
  
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

void Inotify::addSubdir(std::string rel_path, int wd)
{
  this->subdirs.push_back({rel_path, wd});
}

std::vector<path_name> Inotify::listrecursive(std::string path) {
  std::vector<path_name> fileList;
  struct dirent *entry;
  DIR *dp;

  dp = opendir(path.c_str());
  if (dp == NULL) {
    perror("opendir");
  }
  
  std::cout << "IN: " << path << std::endl;
  while((entry = readdir(dp)))
  {
    if(entry->d_name[0] == '.')
      continue;
    
    
    std::cout << entry->d_name << std::endl;
    fileList.push_back({"",std::string(entry->d_name)});
    
    if(entry->d_type == DT_DIR)
    {
      path += "/"+std::string(entry->d_name);
      std::vector<path_name> tempList = listrecursive(std::string(path));
      for ( auto i = tempList.begin(); i != tempList.end(); i++ ) {
	(*i).path.insert(0, std::string(entry->d_name)+"/");
      }
      fileList.insert(fileList.end(), tempList.begin(), tempList.end() );
    }
  }
  closedir(dp);
  return fileList;
}

std::vector< path_name > Inotify::listdir(std::string path, std::string folder)
{
  std::vector<path_name> list = listrecursive(path);
  
  for ( auto i = list.begin(); i != list.end(); i++ ) {
	(*i).path.insert(0, folder+"/");
      }
      
  return list;
}

