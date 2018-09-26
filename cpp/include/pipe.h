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

struct IO {

  template <typename WriteTo>
  static void Read(int fd, WriteTo& write_to){
    auto size=0;
    char buffer[MAX_BUF_LENGTH]; 

    while( (size = read(fd, buffer, MAX_BUF_LENGTH ) ) > 0) 
      write_to(buffer, size);
  }

  template <typename Data>
  static void Write(int fd, Data& data, int size) {
    write(fd, data, size); 
  } 
};

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

void SimpleTransfer(int from, int to) {
  Channel in{from}, out{to}; 
  in.bufferContentTo<Channel>(out);
};

class Tunnel {
  private: 
    int _from, _to;
    using Strategy = function<void(int from, int to)>;
    Strategy incoming = SimpleTransfer ; 
    Strategy outcoming = SimpleTransfer ; 

  public: 
    Tunnel(){

    }

    Tunnel& from(int _f) {
      _from = _f;
      return *this;
    }

    Tunnel& to(int _t) {
      _to = _t;
      return *this;
    }

    void join(){
      thread t1 {incoming, _from, _to };
      thread t2 {outcoming, _to, _from };

      t1.detach();
      t2.detach();
    }

    Tunnel& setIncomingTransferStrategy(Strategy strategy){
      incoming = strategy;
      return *this;
    }
    
    Tunnel& setOutcomingTransferStrategy(Strategy strategy){
      outcoming = strategy;
      return *this;
    }

}; 
