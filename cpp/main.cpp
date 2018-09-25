#include <iostream>
#include <sstream>
#include <ctime>

#include "network.h"
#include "pipe.h"
#include "http.h"

using namespace std;

function<void(void)> timming() {
  auto _start = clock();
  cout << "start" << endl;
  
  return [_start]() {
    auto final_time = (clock() - _start) / (double)(CLOCKS_PER_SEC / 1000);
    cout << "Elapse time: " << final_time  << " ms" << endl; 
  };
}

int main(){
  cout << "thread pool example" << endl;	

  Server server{8080};
  Client client{"localhost", 8087}; 

  server.waitForConnections([&client](int fd_server){
      auto fd_client = client.establishConnection();

      cout << "fd_server: " << fd_server << endl; 
      cout << "fd_client: " << fd_client << endl; 


      Tunnel tunnel;

      auto tmm = timming();
      tunnel.from(fd_server)
            .to(fd_client)
            .probe(tmm)
            .join();




      });

  return 0;
}
