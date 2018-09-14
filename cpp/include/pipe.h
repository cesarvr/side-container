#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <netdb.h>
#include <iostream>
#include <functional>

using namespace std; 
const unsigned int MAX_BUF_LENGTH = 1024;

string Read(int file_descriptor) {
  vector<char> buffer(MAX_BUF_LENGTH); 
  auto read_size = 1; 
  string data; 

  while( read_size > 0 ) {
    read_size = read( file_descriptor, buffer.data(), MAX_BUF_LENGTH );
    data.append(buffer.begin(), buffer.end());

    if((int)data.back() == 0)
      break;
  }

  return data;
}

template <typename FD>
void Write(FD& fd, string&& str) {  
  auto rsize = str.size() * sizeof(char);
  write(fd, str.c_str(), rsize); 
}

string&& Debug(string&& str){
  cout << "comming from the server: " << str << endl;
  return move(str);
}


class Pipe {
  private: 
    struct Options {
      int from; 
      int to; 
    };

    using Output = function<decltype(Debug)>;    
    
    Output output;
    Options opt; 

  public: 
    Pipe(Options _opt): opt{_opt}{}

    ~Pipe(){
      cout << "closing the pipe" << endl;
      close(opt.from);
      close(opt.to);
    }

    void addOutputController(Output&& outDelegate){
      output = outDelegate; 
    }

    string OutputControl(Options opt) {
      if( output == nullptr)
        return Read(opt.to); 

      return output(Read(opt.to));
    }

    void pipe(){
      Write(opt.to,   Read(opt.from));  
      Write(opt.from, OutputControl(opt));  
    }
};

