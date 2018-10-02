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

class FileDescriptor {
  private:
    int fd; 
  public:
    FileDescriptor(int _fd): fd{_fd} {} 

    template <class Writer>
      void bufferContentTo(Writer& destination){
        auto size=0;
        char buffer[MAX_BUF_LENGTH];

        while( (size = read(fd, buffer, MAX_BUF_LENGTH ) ) > 0) {
//          printf("read: [ fd: %d  size: %d] \n", fd, size);
          destination.Write(buffer, size);
        }
      }

    template <typename Buffer>
      void Write(Buffer&& buffer, int size) {
 //       printf("write: [ fd: %d  size: %d] \n", fd, size);
        auto bytes = write(fd, buffer, size); 
      }


    ~FileDescriptor(){
  //    cout << "closing connection: " << fd << endl;
      close(fd);
    }
};  


void aa() {  }

void SimpleTransfer(int from, int to) {
  FileDescriptor in{from}, out{to}; 
  in.bufferContentTo<FileDescriptor>(out);
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

    Tunnel& requestDelegate(Strategy strategy){
      incoming = strategy;
      return *this;
    }
    
    Tunnel& responseDelegate(Strategy strategy){
      outcoming = strategy;
      return *this;
    }

}; 
