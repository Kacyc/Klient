#include <iostream>
#include <poll.h>
#include <string.h>
#include <signal.h>

#include "connector.h"
#include "stream.h"
#include "inotify.h"
#include "filehandlernormal.h"
#include "filehandlerremove.h"
#include "filehandlerfolder.h"
#include "filehandlermove.h"
int FD;
/*
void terminationProtocol(int signal)
{
  Stream stream(FD);
  struct path_name p ={"GOOD","BYE"};
  stream.send_file("", p);
  exit(0);
}
*/


int main(int argc, char **argv) {
    
    const char* dir = "/home/mati/cl";
    if(argv[1] != NULL)
      dir = argv[1];
    //const char* addr =  argv[2];
    //int port = atoi( argv[3]);
    const char* addr = "127.0.0.1";
    int port = 1330;
    
    Inotify inotify(dir);
   
    
    Connector connector;
    int fd = connector.conn(addr, port);
    FD=fd;
    int nfds = 3;
    struct pollfd fds[nfds];	
    fds[0].fd = fd;	
    fds[0].events = POLLIN;
    
    fds[1].fd = 0;
    fds[1].events = POLLIN;
    
    fds[2].fd = inotify.get_fd();
    fds[2].events = POLLIN;
    
    
   
    
    Stream stream(fd, &inotify);
    
    //int br = stream.recv_data(buffer, bufsize);
    
    
    //char buffsend[] = {"Czesc "};
    //signal(SIGINT,terminationProtocol);
    
    while(1)
    {
      int rv = poll(fds, nfds, -1/*3*60*1000*/);
      if(rv == -1){
	perror("poll error");
      }
      else if (rv == 0)
      {
	std::cout << "Timeout poll"  << std::endl;
      }
     

   
	if (fds[0].revents == 1)
	{
	  
	  //inotify.remove_watch();
	  FileHandler* fh = stream.recv_file(dir,&inotify);	//nastopilo zdarzenie od serwera i klient odbiera plik
	  delete fh;
	  //inotify.add_watch();
	  
	  /*if(br==0)
	  {  
	    std::cout << "connection lost: " << nfds << "->" <<nfds-1   << std::endl;
	    nfds--;
	    break;
	  }
	  */
	}
	else if(fds[1].revents == 1)
	{
	  //zdarzenie od standartowego wejscia, uzywane wczesniej do testu wysylania poszczegolnych plikow
	  int bufsize=256;
	  char buffer[bufsize];
	  int bytes_read = read(0,buffer,bufsize);
	  buffer[bytes_read-1] = '\0';
	  std::cout << "read len: " << strlen(buffer) << std::endl;
	  //stream.send_file("/home/mati/test",buffer);
	  //stream.send_message(buffer,bytes_read); 
	}
	else if(fds[2].revents == 1)
	{
	  //zdarzenia od inotify, odbieramy nazwy plikow i wysylamy je do serwera
	  
	  //std::cout << "inotify something happened" << std::endl; 
	 std::vector<FileHandler*> x = inotify.readNotify();
	 for(std::vector<FileHandler*>::iterator it = x.begin(); it != x.end(); ++it) 
	 { 
	   //std::cout << "Przechodze do wysylania: " << (*it).name << std::endl;
	    stream.send_file(dir, (*it), &inotify);
	    delete (*it);
	 }
	 
	}

     }
    
    
    return 0;
}
