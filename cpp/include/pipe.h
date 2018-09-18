#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <netdb.h>
#include <iostream>
#include <functional>
#include <thread>

using namespace std; 
const unsigned int MAX_BUF_LENGTH = 1024;

template <typename T>
T&& Debug(T&& data){
  string tmp; 
  tmp.append(data.begin(), data.end());

  cout << endl;
  cout << "comming from the server: " << data.size() << endl;
  return data;
}

void Tunnel(int fd_in, int fd_out) {
  vector<char> buffer(MAX_BUF_LENGTH); 
  int size=0;
  while( (size = read( fd_in, buffer.data(), MAX_BUF_LENGTH ) ) >0 ) {
      write(fd_out, buffer.data(), size); 
  }

  close(fd_in);
  close(fd_out);
}

struct Channel {
  Channel(int fd_server, int fd_client) {
    thread th  { Tunnel, fd_server, fd_client };
    thread th2 { Tunnel, fd_client, fd_server };

    th.detach();
    th2.detach();
  }
};

