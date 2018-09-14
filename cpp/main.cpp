#include <iostream>
#include "network.h"
#include "pipe.h"
#include <sstream>

using namespace std;

string addCookies(string payload) {
  istringstream data(payload);
  string token, buffer;

  while(std::getline(data, token)) {
    buffer += token + "\n";
    if(token.find("Content-type:")!= string::npos)
      buffer += "Set-Cookie: yummy_cookie=choco\n";
  }

  return buffer;
}

int main(){
  cout << "thread pool example" << endl;	

  Server server{8080};
  Client client{"localhost", 8087}; 

  server.waitForConnections([&client](auto fd_server){
      auto fd_client = client.establishConnection();
  
      Write(fd_client, Read(fd_server)); 
      Write(fd_server, addCookies(Read(fd_client))); 

      close(fd_client);
      close(fd_server);
   });

  return 0;
}
