//# Linux socket headers
#include <sys/types.h> 
#include <sys/socket.h>
#include <netdb.h>

// C++
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
  return data.data();
}

template <typename T>
struct IO {
  T& functor;
  IO<T>(T&& tt): functor{tt} {}

  template <typename... Args>
  auto write(Args... args) -> decltype(write(args...)){
    functor(args...);
    return write(args...);
  }
};

template <>
struct IO<void> {
  template <typename... Args>
    auto write(Args... args) -> decltype(write(args...)){
      return write(args...);
    }
};


class Channel {
  private:
    int fd; 
    char buffer[MAX_BUF_LENGTH]; 
    function<void (char *)> outMonitor;
    function<void (char *)> inMonitor;
    function<void (char *)> endMonitor;


  public:
    Channel(int _fd): fd{_fd} {
    }; 

    void bufferContentTo(Channel& destination){
      auto size=0;
      cout << "read buffer fd:"<< fd << " size:" <<  MAX_BUF_LENGTH << endl;

      while( (size = read(fd, buffer, MAX_BUF_LENGTH ) ) > 0) {
        cout << "read size:  " << size <<endl;
        if(inMonitor != nullptr)
          inMonitor(buffer);

        destination.Write(this, size);
      }
    }

    void setOutBoundMonitor(function<void (char*)> monitor) {
      outMonitor = monitor; 
    }

    
    void setInBoundMonitor(function<void (char*)> monitor) {
      inMonitor = monitor; 
    }

    void setCloseMonitor(function<void (char*)> monitor) {
      endMonitor = monitor;
    }

    void Write(Channel* destination, int size) {
      cout << "write size:  " << size <<endl;

      if(outMonitor != nullptr)  
        outMonitor(destination->data());

      write(fd, destination->data(), size); 
    }

    char* data(){
      return buffer;
    }

    ~Channel(){
      cout << "closing: " << fd << endl;
      close(fd);

      if(endMonitor != nullptr)
        endMonitor(buffer);
    }
};  


template <typename Fn>
void Tunnel(int fd_in, int fd_out, Fn&& fn){
  Channel in{fd_in}, out{fd_out}; 
  out.setOutBoundMonitor(fn);
  in.bufferContentTo(out); 
}

template <typename Fn>
void TunnelMonitorClosingSessionStart(int fd_in, int fd_out, Fn&& fn){
  Channel in{fd_in}, out{fd_out}; 
  fn(nullptr);
  in.setCloseMonitor(fn);
  in.bufferContentTo(out); 
}

template <typename Fn>
void TunnelMonitorClosingSessionEnd(int fd_in, int fd_out, Fn&& fn){
  Channel in{fd_in}, out{fd_out}; 
  in.setCloseMonitor(fn);
  in.bufferContentTo(out); 

  fn(nullptr);
}

void Tunnel(int fd_in, int fd_out){
  Channel in{fd_in}, out{fd_out}; 
  in.bufferContentTo(out); 
}




/*
   void Tunnel(int fd_in, int fd_out) {
   vector<char> buffer(MAX_BUF_LENGTH); 
   auto size=0;
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
   };*/

