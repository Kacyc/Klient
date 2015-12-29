#include "inotify.h"

Inotify::Inotify(const char* path)
{
  this->path=path;
  
  this->fd = inotify_init();

  if ( fd < 0 ) {
    perror( "inotify_init" );
  }
};

std::vector<std::string> Inotify::readNotify()
{
  std::vector<std::string> red;
  char buffer[BUF_LEN];
  int length,i=0;
  std::string line;
  wd = inotify_add_watch( fd, path, IN_MODIFY | IN_CREATE | IN_DELETE );
   length = read( fd, buffer, BUF_LEN );  

  if ( length < 0 ) {
    perror( "read" );
  }  

  while ( i < length ) {
    struct inotify_event *event = ( struct inotify_event * ) &buffer[ i ];
    if ( event->len ) {
      if ( event->mask & IN_CREATE ) {
        if ( event->mask & IN_ISDIR ) {
          printf( "The directory %s was created.\n", event->name );
	  line="The directory "+std::string(event->name)+ " was created";
	  red.push_back(line);
        }
        else {
          printf( "The file %s was created.\n", event->name );
	  line="The file "+std::string(event->name)+ " was created";
	  red.push_back(line);
        }
      }
      else if ( event->mask & IN_DELETE ) {
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
      else if ( event->mask & IN_MODIFY ) {
        if ( event->mask & IN_ISDIR ) {
          printf( "The directory %s was modified.\n", event->name );
	  line="The directory "+std::string(event->name)+ " was modified";
	  red.push_back(line);
        }
        else {
          printf( "The file %s was modified.\n", event->name );
	  line="The file "+std::string(event->name)+ " was modified";
	  red.push_back(line);
        }
      }
    }
    i += EVENT_SIZE + event->len;
  }
  ( void ) inotify_rm_watch( fd, wd );
  return red;
}
