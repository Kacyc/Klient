#include "inotify.h"

Inotify::Inotify(const char* path)
{
  this->path=path;
  
  this->fd = inotify_init();
  
  if ( fd < 0 ) {
    perror( "inotify_init" );
  }
  
  this->wd = inotify_add_watch( fd, path, IN_CLOSE_WRITE |  IN_CREATE | IN_DELETE |IN_MOVE);

};


std::vector<std::string> Inotify::readNotify()
{
  std::vector<std::string> red;
  char buffer[BUF_LEN];
  int length,i=0;
  std::string line;
  
  
  length = read( fd, buffer, BUF_LEN );  
  
  if ( length < 0 ) {
    perror( "read" );
  }  
  
  
  while (i < length) {
    struct inotify_event *event = ( struct inotify_event * ) &buffer[ i ];
    if ( event->len && event->name[0] != '.' ) {
      if (event->mask & IN_CLOSE_WRITE && !has_suffix(event->name,".part") ) {
        if ( event->mask & IN_ISDIR ) {
          printf( "The directory %s was created/modified.\n", event->name );
	  std::string rel_path = get_rel_path(event->wd, std::string(event->name));
	  red.push_back(rel_path);
        }
        else {
          printf( "The file %s was created/modified.\n", event->name );
	  std::string rel_path = get_rel_path(event->wd, std::string(event->name));
	  red.push_back(rel_path);
        }
      }
      else if ( event->mask & IN_DELETE && !has_suffix(event->name,".part")) {
        if ( event->mask & IN_ISDIR ) {
          printf( "The directory %s was deleted.\n", event->name ); 
	  line="The directory "+std::string(event->name)+ " was deleted"; 
	  std::string rel_path = get_rel_path(event->wd, std::string(event->name));
	  red.push_back(rel_path);
        }
        else {
          printf( "The file %s was deleted.\n", event->name );
	  line="The file "+std::string(event->name)+ " was deleted"; 
	  std::string rel_path = get_rel_path(event->wd, std::string(event->name));
	  red.push_back(rel_path);
        }
      }
      else if ( event->mask & IN_CREATE && event->mask & IN_ISDIR && !has_suffix(event->name,".part")) {
        if ( event->mask & IN_ISDIR ) {
          printf( "The directory %s was created.\n", event->name );
	  std::string rel_path = get_rel_path(event->wd, std::string(event->name));
	  std::string new_abs_path = std::string(path) + "/" + rel_path; 
	  int new_wd = inotify_add_watch( fd, new_abs_path.c_str(), IN_CLOSE_WRITE | IN_CREATE | IN_DELETE ); 
	  std::cout << "Dodaje nowy folder o sciezce absolutnej: " << new_abs_path << std::endl;
	  std::cout << "Dodaje nowy folder o sciezce wzglednej: " << rel_path << std::endl;
	  subdirs.push_back({rel_path, new_wd});
	  red.push_back(rel_path);
        }/*
        else {
          printf( "The file %s was modified.\n", event->name );
	  red.push_back(std::string(event->name));
        }*/
      }
      else if ( event->mask & IN_MOVED_FROM ) {
         if(!has_suffix(event->name,".part"))
	 {
	  printf( "The file/directory %s was moved from.\n", event->name ); 
	  std::string rel_path = get_rel_path(event->wd, std::string(event->name));
	  red.push_back(rel_path);
	 }
	 else
	  cookies.push_back(event->cookie);
        
      }
      else if ( event->mask & IN_MOVED_TO && !has_suffix(event->name,".part")){
        if( !is_cookie_on_vector(event->cookie) )
	{
	 printf( "The file/directory %s was moved to.\n", event->name ); 
	 std::string rel_path = get_rel_path(event->wd, std::string(event->name));
	 red.push_back(rel_path);
	}
	else
	  delete_cookie(event->cookie);
	
	  
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
	    rel_path = (*it).path + "/" + event_name;
  }
  else
    rel_path = event_name;
  
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