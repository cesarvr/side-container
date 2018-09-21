#include <iostream>
#include <sstream>
#include <ctime>

#include "network.h"
#include "pipe.h"
#include "http.h"

using namespace std;

int randomNumber() {
  srand (time(NULL));
  return rand() % 1000 + 1; 
}



int main(){
  cout << "thread pool example" << endl;	

  Server server{8080};
  Client client{"localhost", 8087}; 

  server.waitForConnections([&client](int fd_server){
      auto fd_client = client.establishConnection();

      cout << "fd_server: " << fd_server << endl; 
      cout << "fd_client: " << fd_client << endl; 


      
      clock_t start;
      auto timmingStart = [&start](char *data){
        start = clock(); 
      };

      auto elapsedTime = [&start](char *data){
        cout << "Time: " << (clock() - start) / (double)(CLOCKS_PER_SEC / 1000) << " ms" << endl; 
      };

      thread receive  { TunnelMonitorClosingSessionStart<decltype(timmingStart)> ,fd_server, fd_client, timmingStart };  
      thread response { TunnelMonitorClosingSessionEnd<decltype(elapsedTime)>,fd_client, fd_server, elapsedTime };  

      receive.detach();
      response.detach();

      });

  return 0;
}
