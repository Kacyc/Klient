#include <poll.h>
#include<algorithm>
#include "acceptor.h"
#include "inotify.h"
#include "stream.h"

#include "filehandlernormal.h"
#include "filehandlerremove.h"
#include "filehandlerfolder.h"
#include "filehandlermove.h"

int main(int argc, char **argv) {

    if(argc < 4)
    {
      std::cout << "Podaj wszystkie argumenty: 1-IP 2-port 3-folder" << std::endl;
      return 1;
    }
        
    const char* addr = argv[1];
    int port = atoi( argv[2]);
    if(port < 1024 || port > 65536){
      std::cout << "Niewlasciwy port" << std::endl;
      return 1;
    }
    const char* dir = argv[3];

    Acceptor acceptor(addr, port);
    acceptor.start();	//bind,listen itd...



    int nfds = 1;
    struct pollfd fds[32];
    fds[0].fd = acceptor.get_fd(); //deskryptor na odbior z gniazda
    fds[0].events = POLLIN;


    Stream* stream;
    Stream* new_stream;




    while(1)
    {
        int rv = poll(fds, nfds, -1);
        if(rv == -1)
            perror("poll error");
        else if (rv == 0)
            std::cout << "Timeout poll"  << std::endl;
        

        int curr_nfds=nfds;
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
                int emptyindex =-1;
                for (int j = 0; j<nfds; j++)
                {
                    if (fds[j].fd==-1)
                    {
                        emptyindex = j;
			break;
                    }
                }
                if (emptyindex == -1)
                {
                    fds[nfds].fd = new_fd;
                    fds[nfds].events = POLLIN;
                    nfds++;
                } else
                {
                    fds[emptyindex].fd=new_fd;
                }
                fds[i].revents=0;
                std::cout << "Nazwiazano polaczenie " << nfds-1 << "->" << nfds  << std::endl;
                
		//wysylamy nowo podlaczonemu klientowi pliki serwera
                new_stream = new Stream(new_fd,dir);
		new_stream->sendInitFiles();
                delete new_stream;
            }
            else
            {
                //nastapilo zdarzenie od klienta
                stream = new Stream(fds[i].fd, dir);


                //odbieramy plik od klienta
                FileHandler* file_to_send = stream->recv_file();
                if(file_to_send->getType()==4)
                {
                  std::cout<<"Rozlaczenie klienta"<<std::endl;
                  fds[i].fd=-1;
                } else
                {
                
		  //odebrany plik wysylam pozostalym klientom
		  for(int j=0 ; j < nfds ; j++)
		  {

		    if( (fds[j].fd != stream->get_fd()) && (fds[j].fd != acceptor.get_fd()) && (fds[j].fd !=-1 ) )
                    {
                        std::cout << "Wysylanie: " << file_to_send->getName() << " do deskryptora j: " << j << std::endl;
                        new_stream = new Stream(fds[j].fd,dir);
                        new_stream->send_file(file_to_send);

                        delete new_stream;
                    }

		  }
		}

                delete file_to_send;
                delete stream;
            }

        }



    }
    return 0;
}
