#include <iostream>
#include <sstream>
#include <ctime>
#include <functional>

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

struct Clock {
  clock_t _start; 
  
  Clock(){
    _start = clock();
  }

  void now(){
    auto final_time = (clock() - _start) / (double)(CLOCKS_PER_SEC / 1000);
    cout << "Elapse time: " << final_time  << " ms" << endl; 
  }

  ~Clock(){
    cout << "Clock: destroyed" << endl;
  }
};

void ouut(Clock& et, int fd_in, int fd_out){
  SimpleTransfer(fd_in, fd_out); 
  et.now();
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

      Clock et; 

      auto out_strategy = bind(ouut, et, placeholders::_1, placeholders::_2);

      tunnel.from(fd_server)
      .setOutcomingTransferStrategy(out_strategy)
      .to(fd_client)
      .join();
  });

  return 0;
}
