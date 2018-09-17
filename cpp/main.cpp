#include <iostream>
#include <sstream>

#include "network.h"
#include "pipe.h"
#include "http.h"

using namespace std;

int randomNumber() {
  srand (time(NULL));
  return rand() % 1000 + 1; 
}

string addCookies(string&& payload) {
  HTTPMessage httpMessage{payload};

  httpMessage.getHeaders().add("Set-Cookie", "visitor=232");
 
  cout << "response:" << httpMessage.toString() << endl; 

  return httpMessage.toString();
}

int main(){
  cout << "thread pool example" << endl;	

  Server server{8080};
  Client client{"localhost", 8087}; 

  server.waitForConnections([&client](int fd_server){
    auto fd_client = client.establishConnection();

    Channel ch{fd_server, fd_client}; 
  });

  return 0;
}
