#include <iostream>
#include "server.h"
using namespace std;

void handle(int file_descriptor){
  string resp = "HTTP/1.0 200 OK \n\n Date: Mon, 11 July 2016 23:59:59 GMT\n Content-Type: text/html  \n\n <html> <head> <title> Hi!! </title>  </head> <body> <h1>Hello World</h1> </body>  </html> \n";

  cout << "data received ->" << Read(file_descriptor) << endl;
  Write(file_descriptor, resp);

  cout << "closing connection..." << endl;
  close(file_descriptor);
}

int main(){
  std::cout << "thread pool example" << std::endl;	
  
  Server server{8080};
  server.waitForConnections(handle);

  return 0;
}


