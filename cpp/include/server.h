#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <unistd.h>    //write
#include <sys/epoll.h>

#include <vector>
#include <iostream>
#include <thread> //c++11 threads.

using namespace std;

string Read(int file_descriptor) {
  const unsigned int MAX_BUF_LENGTH = 1024;
  std::vector<char> buffer(MAX_BUF_LENGTH); 
  auto read_size = 1; 
  std::string data; 

  while( read_size > 0 ) {
    read_size = read( file_descriptor, buffer.data(), MAX_BUF_LENGTH );
    data.append(buffer.begin(), buffer.end());
    auto b = data.back();
    if((int)b == 0)
      break;
  }

  return data;
}

template <typename FD>
void Write(FD& fd, string str) {  
  auto rsize = str.size() * sizeof(char);
  write(fd, str.c_str(), rsize); 
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

class Server {
private:
  int socket_descriptor, port;
  struct sockaddr_in server , client;

public:
  Server(int port): port{port}{
    //Create socket
    socket_descriptor = socket(AF_INET , SOCK_STREAM , 0);

    this->check_error("Can't create a socket", socket_descriptor);

    puts("Socket created");

    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( port );
  }

  void check_error(string &&mess, int code){
    if(code == -1)
    {
      //print the error message
      perror(mess.c_str());
      exit (EXIT_FAILURE);
    }
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


