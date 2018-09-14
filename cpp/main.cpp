#include <iostream>
#include "network.h"
#include "pipe.h"
#include <sstream>

using namespace std;

string&& addCookies(string&& payload) {
  istringstream data(payload);
  string token, buffer;

  while(std::getline(data, token)) {
    buffer += token + "\n";
    if(token.find("Content-type:")!= string::npos)
      buffer += "Set-Cookie: yummy_cookie=choco\n";
  }

  return move(buffer);
}

int main(){
  cout << "thread pool example" << endl;	

  Server server{8080};
  Client client{"localhost", 8087}; 

  server.waitForConnections([&client](auto fd_server){
      auto fd_client = client.establishConnection();

      Pipe pipe {{.from=fd_server, .to=fd_client }}; 
      pipe.addOutputController(addCookies);
      pipe.pipe();
      });

  return 0;
}
