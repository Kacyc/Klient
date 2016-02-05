#include <poll.h>
#include "acceptor.h"
#include "inotify.h"
#include "stream.h"
#include <mutex>

int main(int argc, char **argv) {
    
  
    const char* dir = "/home/mati/sv";
    if(argv[1] != NULL)
      dir = argv[1];
    
    
    const char* addr = "127.0.0.1";
    int port = 1330;
    int nconnections = 1;

    Inotify inotify(dir);
    
    //przechowujemy listę plików
    std::vector<path_name> filesList;
    
    
    
    Acceptor acceptor(addr, port);
    acceptor.start();	//bind,listen itd...
    
    
    
    int nfds = 1;
    struct pollfd fds[32];
    fds[0].fd = acceptor.get_fd(); //deskryptor na odbior z gniazda
    fds[0].events = POLLIN;
    
   
    Stream* stream;
    Stream* new_stream;
    
    
    
    //stream.send_data(buffsend, sizeof(buffsend)/sizeof(char));
    
    
    
    while(nfds)
    {
      int rv = poll(fds, nfds, 3*60*1000);
      if(rv == -1){
	perror("poll error");
      }
      else if (rv == 0)
      {
	std::cout << "Timeout poll"  << std::endl;
      }
     
     int curr_nfds = nfds;
     for(int i = 0 ; i < curr_nfds ; i++)
     {
	if(fds[i].revents == 0)
	{
	  continue;
	}
	
	if (fds[i].fd == acceptor.get_fd())
	{
	  //klient chce sie polaczyc i dodajemy jego deskryptor do tablicy deskryptorow
	  int new_fd = acceptor.accp();
	  fds[nfds].fd = new_fd;
          fds[nfds].events = POLLIN;
          nfds++;
	  fds[i].revents=0;
	  std::cout << "nazwiazano polaczenie " << nfds-1 << "->" << nfds  << std::endl;
	  
	  //TODO i teraz wrzucamy wszystkie pliki z listy do tego klienta
	  
	  new_stream = new Stream(new_fd);
	for(std::vector<path_name>::iterator it = filesList.begin(); it != filesList.end(); ++it) 
	{ 
	  std::cout << "Zaraz wysylam " << (*it).name << " do deskrpytora j: " << new_fd << std::endl;
	  new_stream->send_file(dir,*it);
	}
	delete new_stream;  
	}
	else
	{
	  //nastapilo zdarzenie od klienta
	  stream = new Stream(fds[i].fd);
	  int bufsize = 256;
	  char buffer[bufsize];
	  struct data d;
	  
	  //odbieramy plik od klienta
	  path_name file_to_send = stream->recv_file(dir);
	  
	  //Jeśli plik nie znajduje się na liście, dodajemy go
	  std::vector<path_name>::iterator it;
	  for(it = filesList.begin(); it != filesList.end(); ++it) 
	  { 
	    if (it->path==file_to_send.path&&it->name==file_to_send.name)
	      break;
	  }
	  if (it == filesList.end())
	    filesList.push_back(file_to_send);
	  
	  //wysylamy odebrany plik pozostalym klientom
	  for(int j=0 ; j < curr_nfds ; j++)
	  {
	    
	    if( (fds[j].fd != stream->get_fd()) && (fds[j].fd != acceptor.get_fd() ) )
	    {
	      std::cout << "Zaraz wysylam " << file_to_send.name << "do deskrpytora j: " << j << std::endl;
	      new_stream = new Stream(fds[j].fd);
	      new_stream->send_file(dir,file_to_send);
	      
	      delete new_stream;
	    }
	    
	  }
	  
	  //int br = stream->recv_message(buffer, bufsize);
	  
	  
	  /*if(br==0)
	  {  
	    std::cout << "connection lost: " << nfds << "->" <<nfds-1   << std::endl;
	    nfds--;
	    break;
	  }
	    */
	  delete stream;
	  
	  
	}
	
     }
      
    }
    return 0;
}