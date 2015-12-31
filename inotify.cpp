#include "inotify.h"

Inotify::Inotify(const char* path)
{
  this->path=path;
  
  this->fd = inotify_init();
  
  if ( fd < 0 ) {
    perror( "inotify_init" );
  }
  
  this->wd = inotify_add_watch( fd, path, IN_MODIFY | IN_CREATE | IN_DELETE );

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
    if ( event->len && event->name[0] != '.') {
      if ( event->mask & IN_CREATE ) {
        if ( event->mask & IN_ISDIR ) {
          printf( "The directory %s was created.\n", event->name );
	  red.push_back(std::string(event->name));
        }
        else {
          printf( "The file %s was created.\n", event->name );
	  red.push_back(std::string(event->name));
        }
      }
      else if ( event->mask & IN_DELETE && event->name[0] != '.') {
        if ( event->mask & IN_ISDIR ) {
          printf( "The directory %s was deleted.\n", event->name ); 
	  line="The directory "+std::string(event->name)+ " was deleted"; 
	  red.push_back(line);
        }
        else {
          printf( "The file %s was deleted.\n", event->name );
	  line="The file "+std::string(event->name)+ " was deleted"; 
	  red.push_back(line);
        }
      }
      else if ( event->mask & IN_MODIFY && event->name[0] != '.') {
        if ( event->mask & IN_ISDIR ) {
          printf( "The directory %s was modified.\n", event->name );
	  red.push_back(std::string(event->name));
        }
        else {
          printf( "The file %s was modified.\n", event->name );
	  red.push_back(std::string(event->name));
        }
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