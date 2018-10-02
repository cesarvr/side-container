#include <iostream>
#include <math.h>
#include <string>
#include <sstream>
#include <vector>
#include <map>

using namespace std;
const string COOKIE = "Set-Cookie"; 
const string CONTENT_TYPE = "Content-type";


class HTTPHeaders {
  private:
    using Value = pair<string, string>;
    map<string, string> headers;
    vector<Value> ordered_list;
    vector<Value> cookies;
    int offset = 0;

    void setup_cookies(ostringstream& output){
      for(auto cookie : cookies)
        output << cookie.first << ": " << cookie.second << "\n";    
    }

  public:

    HTTPHeaders(){}
    HTTPHeaders(vector<Value> _list): ordered_list {_list}{
      for(Value& value: ordered_list)
        headers[value.first] = value.second;
    }

    void addCookie(string value){
      cookies.push_back(make_pair(COOKIE, value));
    }

    void edit(string key, string _value){
      auto value = headers[key];
      headers[key] = _value;
    }

    string getContentType() {
      return headers[CONTENT_TYPE];
    }

    vector<Value>& ordered(){
      return ordered_list;
    }

    static bool isHeader(string property) {
      return ( property.find(":") != string::npos );
    }

    static Value getValue(string data){
      Value header;
      auto sc_position = data.find(":");
      auto key   = data.substr(0,sc_position);
      auto value = data.substr(sc_position+2, data.length());
      header = make_pair(string(key), string(value));

      return header;
    }

    static HTTPHeaders parse(string headers_block){
      istringstream tokens {headers_block};
      string token;
      vector<Value> ordered_list;

      while(getline(tokens, token))
        if(isHeader(token)) {
          auto value = getValue(token);
          ordered_list.push_back(value);
        }

      return HTTPHeaders{ordered_list};
    }

    string stringify() {
      ostringstream output;
      output << "Server"  << ": " << headers["Server"]  << "\n";
      output << "Date"    << ": " << headers["Date"]  << "\n";
      output << "Content-type"    << ": " << headers["Content-type"]  << "\n";
      setup_cookies(output);

      output << "Content-Length"  << ": " << headers["Content-Length"]  << "\n";
      auto _output = output.str();
      return _output;
    }
};

struct Status {
  string version;
  string code;
  string description;

  string toString(){
    return version +" "+ code + " " + description + "\r\n";
  }
};

struct HTTPStatus {
  string html;

  static Status getStatusHeader(string _htmlData) {
    istringstream html(_htmlData);
    Status status;
    html >> status.version;
    html >> status.code;
    html >> status.description;

    return status;
  }

  static string extractFirstOccurrence(string data, string query) {
    auto pos = data.find(query);
    if(pos != string::npos) {
      return data.substr(0, pos);
    }
    return "";
  }

  static bool isValidStatusHeader(string _htmlData) {
    istringstream html(_htmlData);
    string token;
    html >> token;
    if(token.find("HTTP/") != string::npos)
      return true;
    return false;
  }

  static bool isHTTPStatusHeader(string html) {
    auto header = extractFirstOccurrence(html, "\n");
    if(header!="")
      return isValidStatusHeader(header);
    return false;
  }

  static string stringify(Status& status) {
    return status.version + " "+ status.code + " " + status.description + "\r\n";
  }

};


class HTTP{
  private:
    HTTPHeaders headers;
    Status status;
    string content;
    bool isNewMessage; 
    int raw_size;
    ostringstream output;
  public:

    HTTP(string message) {
      raw_size = message.size(); 
      isNewMessage = HTTPStatus::isHTTPStatusHeader(message);

      if(isNewMessage) {
        status = HTTPStatus::getStatusHeader(message);
        auto http_separation = message.find("\n\r");
        cout << endl;

        if( http_separation != string::npos ) {
          auto raw_header = message.substr(0, http_separation); 
          headers = HTTPHeaders::parse(raw_header);

          content = message.substr(http_separation+1, message.length()); 
        }else
          cout << "didn't found any headers!" << endl;
      }
      else{
        content = message; 
      }
    }

    bool isRawData(){
      return !isNewMessage;
    }

    int calculateOffset(int size){
      if(isNewMessage){
        auto _out = output.str(); 
        double offset = (_out.size() * (size+1)) / raw_size; 
        cout << "offset: " << offset << endl;
        cout << "raw_size: " << raw_size << endl;
        return ceil(offset);
      }else{
        return size;
      }
    }

    HTTPHeaders& getHeaders(){
      return headers;
    }

    string toString() {
      if(isNewMessage) {
        output << HTTPStatus::stringify(status);
        output << headers.stringify();
        output << content;
        return output.str();
      }else{ 
        output << content;
        return output.str();
      }
    }
};


