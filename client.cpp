#include <iostream>
#include <poll.h>
#include <string.h>
#include "connector.h"
#include "stream.h"
#include "inotify.h"
#include <signal.h>

int FD;

void terminationProtocol(int signal)
{
  Stream stream(FD);
  struct path_name p ={"GOOD","BYE"};
  stream.send_file("", p);
  exit(0);
}



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
    int nfds = 4;
    struct pollfd fds[nfds];	
    fds[0].fd = fd;	
    fds[0].events = POLLIN;
    
    fds[1].fd = 0;
    fds[1].events = POLLIN;
    
    fds[2].fd = inotify.get_fd();
    fds[2].events = POLLIN;
    
    
   
    
    Stream stream(fd, &inotify);
    
    int bufsize = 256;
    char buffer[bufsize];
    char buffsend[24];
    //strncpy (buffer, date.c_str(), date.length() );
    //stream.send_message(buffer, bufsize);
    
    //int br = stream.recv_data(buffer, bufsize);
    
    
    //char buffsend[] = {"Czesc "};
    signal(SIGINT,terminationProtocol);
    
    while(1)
    {
      int rv = poll(fds, nfds, 3*60*1000);
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
	  stream.recv_file(inotify.get_path());	//nastopilo zdarzenie od serwera i klient odbiera plik
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
	  
	  std::cout << "inotify something happened" << std::endl; 
	 std::vector<path_name> x = inotify.readNotify();
	 for(std::vector<path_name>::iterator it = x.begin(); it != x.end(); ++it) 
	 { 
	   std::cout << "Przechodze do wysylania: " << (*it).name << std::endl;
	    stream.send_file(inotify.get_path(), *it);
	  
	 }
	 
	}
     }
    /*
    std::cin >> buffsend;
    std::cout << "Preparing to send" << std::endl;
    stream.send_data(buffsend, sizeof(buffsend)/sizeof(char));
    
    
    
    std::cout << "Sent " <<std::endl;
    */
    
    return 0;
}
