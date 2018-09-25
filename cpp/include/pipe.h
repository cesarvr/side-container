//# Linux socket headers
#include <sys/types.h> 
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

// C++
#include <iostream>
#include <functional>
#include <thread>

using namespace std; 
const unsigned int MAX_BUF_LENGTH = 1024;

class Channel {
  private:
    int fd; 
    char buffer[MAX_BUF_LENGTH]; 


  public:
    Channel(int _fd): fd{_fd} {} 

    template <class Writer>
    void bufferContentTo(Writer& destination){
      auto size=0;

      while( (size = read(fd, buffer, MAX_BUF_LENGTH ) ) > 0) {
        destination.Write(buffer, size);
      }
    }
   
    template <typename Buffer>
    void Write(Buffer&& data, int size) {
      write(fd, data, size); 
    }

    char* data(){
      return buffer;
    }

    ~Channel(){
      close(fd);
    }
};  


void aa() {  }
 
class Tunnel {
  private: 
    int _from, _to;
    function<void(void)> fn = aa; 
    function<void(void)> on_start = aa; 
    function<void(void)> on_end   = aa; 
    
  public: 

    Tunnel& from(int _f) {
      _from = _f;
      return *this;
    }

    Tunnel& to(int _t) {
      _to = _t;
      return *this;
    }

    void join(){

      auto traffic = [](int from, int to, function<void(void)> fn) {
        Channel in{from}, out{to}; 
        in.bufferContentTo<Channel>(out);

        fn();
      };

      thread t1 {traffic, _from, _to, fn };
      thread t2 {traffic, _to, _from , fn};

      t1.detach();
      t2.detach();
    }

      Tunnel& probe(function<void(void)> intercept){
        fn = intercept;
        return *this;
      }
}; 
