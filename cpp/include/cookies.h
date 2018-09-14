#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <cstdlib>

struct Cookie {
  struct Field {
    string key;
    string value;
  };

  vector<Field> fields;

  void debug() {
    cout << "debug" << endl;
    for(auto field: fields)
      cout << "key-> " << field.key << " val->" << field.value << endl;
  }

  Field createValue(string token) {
    auto location = token.find("=");
    auto key = token.substr(0, location);
    auto value = token.substr(location+1, token.size());
    return { .key=key, .value=value };
  }

  bool checkHeaders(string& header, string&& lookFor) {
    auto match = header.find(lookFor);
    if(match != string::npos) {
      return true;
    }
    return false;
  }

  void readCookies(string header) {
    std::istringstream http_header(header);
    vector <string> tokens;

    while(!http_header.eof()){
      string raw_cookie;
      http_header >> raw_cookie;
      if(raw_cookie.find("=") != string::npos)
        tokens.push_back(raw_cookie);
    }

    for(auto token: tokens)
      fields.push_back( createValue(token) );
  }

  bool parseHeader(string httpPayload) {
    std::istringstream http(httpPayload);
    std::string line;

    while(std::getline(http, line)) {
      if(checkHeaders(line, "Cookie")) {
        readCookies(line);
      }
    }

    return true;
  }

  string toString() {
    string buffer = "";
    for(auto field: fields)
      buffer += "Set-Cookie: " + field.key + "=" + field.value + "\n";

    return buffer;
  }

  Cookie(vector<Field> _field): fields{_field}{ }
  Cookie(){ }
};
