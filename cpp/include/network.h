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

void checkForErrors( int code, string mess){
  if(code == -1)
  {
    //print the error message
    perror(mess.c_str());
    exit (EXIT_FAILURE);
  }
}

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
    struct sockaddr_in configuration;
    
  public:
    Server(int port): port{port}{
      
      socket_descriptor = socket(AF_INET , SOCK_STREAM , 0);

      checkForErrors(socket_descriptor, "Can't create a socket");

      puts("Socket created");

      //Prepare the sockaddr_in structure
      configuration.sin_family = AF_INET;
      configuration.sin_addr.s_addr = INADDR_ANY;
      configuration.sin_port = htons( port );
    }

    template <typename Callback>
      void waitForConnections(Callback&& cb) {

        auto status = bind(socket_descriptor, (struct sockaddr *)&configuration , sizeof(configuration));
        checkForErrors(status, "Error binding the socket");
        cout << "server listening in port: " << port << endl; 

        listen(socket_descriptor , 3);

        auto len = sizeof(struct sockaddr_in);  
        while(true)
        { 
          struct sockaddr address;

          // blocking here. 
          auto socket  = accept(socket_descriptor, 
              (struct sockaddr *)&address, (socklen_t *) &len );

          cout << "connection accepted" << endl;

          cb(socket);
        }
      }

    ~Server() {
    }
};


