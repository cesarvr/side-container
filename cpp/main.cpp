#include <iostream>
#include <sstream>
#include <ctime>
#include <functional>
#include <string.h>

#include "tools.h"
#include "network.h"
#include "pipe.h"
#include "http.h"

using namespace std;

struct Clock {
  clock_t _start; 
  
  Clock(){
    _start = clock();
  }

  void now(){
    auto final_time = (clock() - _start) / (double)(CLOCKS_PER_SEC / 1000);
    cout << "Elapse time: " << final_time  << " ms" << endl; 
  }
};



struct Adapter {
  FileDescriptor fd_output; 
  Adapter(int fd): fd_output{fd} {};  

  template <typename Buffer>
  void Write(Buffer& buffer, int size){
    HTTP req{buffer};

    req.getHeaders().edit("Server", "AAA/0.1"); 
    if(req.getHeaders().getContentType().find("html") != string::npos)
    {
      req.getHeaders().addCookie("visitor=6");
      req.getHeaders().addCookie("location=yx");
    }

    auto data = req.toString(); 

    fd_output.Write(data.c_str(), req.calculateOffset(size));
  }
};


void ouut(Clock& et, int fd_in, int fd_out){
  FileDescriptor in{fd_in};
  Adapter out{fd_out};

  in.bufferContentTo(out);
  et.now();
}

int main(){
  cout << "thread pool example" << endl;	
  Server server{8080};
  Client client{"localhost", 8087}; 

  server.waitForConnections([&client](int fd_server){
      auto fd_client = client.establishConnection();
      
      Tunnel tunnel;
      Clock et; 

      auto out_strategy = bind(ouut, et, placeholders::_1, placeholders::_2);

      tunnel.from(fd_server)
      .responseDelegate(out_strategy)
      .to(fd_client)
      .join();
  });

  return 0;
}
