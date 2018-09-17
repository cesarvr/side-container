#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <netdb.h>
#include <iostream>
#include <functional>
#include <thread>

using namespace std; 
const unsigned int MAX_BUF_LENGTH = 1024;

template <typename FD>
void Write(FD fd, vector<char>& buffer) {  
  write(fd, buffer.data(), buffer.size()); 
}

class tguard {
	std::vector<std::thread> pool; 
   
   public:
     tguard(){};
      
      void add_worker(std::thread&& th){
      	pool.push_back(std::move(th));
      } 

   ~tguard(){
    cout << "detaching threads " << endl; 
    
    for(auto &th: pool)
      th.detach();
   }
};

void Tunnel(int fd_in, int fd_out) {
  vector<char> buffer(MAX_BUF_LENGTH); 

  while( (read( fd_in, buffer.data(), MAX_BUF_LENGTH ) ) >0 ) {
      Write(fd_out, buffer); 
  }

  close(fd_in);
  close(fd_out);
}

struct Channel {
  Channel(int fd_server, int fd_client) {
    thread th { Tunnel, fd_server, fd_client };
    thread th2{ Tunnel, fd_client, fd_server };
    tguard workers; 

    workers.add_worker(move(th));
    workers.add_worker(move(th2));
  }
};

string Read(int file_descriptor) {
  vector<char> buffer(MAX_BUF_LENGTH); 
  auto read_size = 1; 
  string data; 

  while( read_size > 0 ) {
    read_size = read( file_descriptor, buffer.data(), MAX_BUF_LENGTH );
    data.append(buffer.begin(), buffer.end());

    cout << "Read: " << data << " size: " << read_size <<  endl;
  }

  cout << "Read:Bye" << endl; 
  return data;
}



string&& Debug(string&& str){
  cout << "comming from the server: " << str << endl;
  return move(str);
}



