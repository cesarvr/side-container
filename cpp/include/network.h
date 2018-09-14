#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <unistd.h>    //write
#include <sys/epoll.h>
#include <netdb.h>

#include <vector>
#include <iostream>
#include <thread> //c++11 threads.

using namespace std;


void checkForErrors( int code, string &&mess){
  if(code == -1)
  {
    //print the error message
    perror(mess.c_str());
    exit (EXIT_FAILURE);
  }
}



class Workers {
  private:
    std::vector<std::thread> pool; 

  public:
    Workers(){};

    void add(std::thread&& th){
      pool.push_back(std::move(th));
    } 

    ~Workers() {
      for(auto &worker : pool){
        cout << "cleaning.." << endl;
        worker.join();
      }
    }
};

class Client {
  private:
    struct sockaddr_in client;
    struct hostent *hostname;

  public: 
    Client(string _hostname, int port){ 
      hostname = gethostbyname(_hostname.c_str());

      if(hostname == nullptr) 
        checkForErrors(-1, "Failing to resolve hostname"); 

      client.sin_family      = AF_INET;
      client.sin_port        = htons(port);
      client.sin_addr.s_addr = *((unsigned long *)hostname->h_addr); 
    }      

    int establishConnection() {
      auto file_descriptor = socket(AF_INET, SOCK_STREAM, 0); 
      checkForErrors(file_descriptor, "There is a problem obtaining stream socket.");

      connect(file_descriptor, (struct sockaddr *)&client, sizeof(client));
      checkForErrors(file_descriptor, "Connection problem.");

      cout << "client connected " << endl;

      return file_descriptor;
    }
};

class Server {
  private:
    int socket_descriptor, port;
    struct sockaddr_in server , client;

  public:
    Server(int port): port{port}{
      //Create socket
      socket_descriptor = socket(AF_INET , SOCK_STREAM , 0);

      checkForErrors(socket_descriptor, "Can't create a socket");

      puts("Socket created");

      //Prepare the sockaddr_in structure
      server.sin_family = AF_INET;
      server.sin_addr.s_addr = INADDR_ANY;
      server.sin_port = htons( port );
    }

    template <typename Callback>
      void waitForConnections(Callback&& cb) {
        //Bind
        auto status = bind(socket_descriptor, (struct sockaddr *)&server , sizeof(server));
        cout << "listening in port " << port << endl; 

        //Listen
        listen(socket_descriptor , 3);

        int len = sizeof(struct sockaddr_in);  
        Workers workers;

        while(true)
        { 
          struct sockaddr address;

          // blocking here. 
          auto socket  = accept(socket_descriptor, 
              (struct sockaddr *)&address, (socklen_t *) &len );
          cout << "connection accepted" << endl;
          //std::thread worker{ cb, socket };	
          cb(socket);
        }
      }

    ~Server() {
    }
};


