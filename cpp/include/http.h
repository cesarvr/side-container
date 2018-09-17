#include <iostream>
#include <string>
#include <sstream>
#include <vector>

using namespace std;

class HTTPHeader {
private:
    vector<pair<string, string>> headers;
    bool loaded = false;
public:
    
    void parseLine(string token) {
        if(!isHeader(token))
            throw "Invalid header line: " + token;
            
        auto colon = token.find(":");

        auto key = token.substr(0, colon);
        auto value = token.substr(colon+1, token.length());
        add(key, value);
    }
    
    bool isLoaded() {
        return loaded;
    }
    
    static bool isHeader(string token){
        auto colon = token.find(":");
        
        if(colon != string::npos) {
            return true;
        }
        return false;
    }
    
    void add(string key, string value) {
        loaded = true;
        headers.push_back(make_pair(key, value));
    }
    
    string toString() {
        string block;
        for(auto header: headers)
            block += header.first + ": " + header.second + " \n";
        
        block += "\n";
        return block;
    }
};

struct HTTPStatus {
    string version;
    string code;
    string description;

    HTTPStatus(string _version, string _code, string _description):
    version{_version},
    code{_code},
    description{_description} { }
    
    HTTPStatus(){}
    
    string toString() {
        return version +" "+code+" "+description + "\n";
    }
};

class HTTPMessage{
private:
    istringstream payload;
    HTTPHeader headers;
    HTTPStatus status;
    string content;
   
public:
    HTTPMessage(string message): payload{message} {
        string line;
        auto headersSection = true;
        auto contentSection = false;
        
        parseHTTPStatus(payload);
 
        while(getline(payload, line)) {
            if(line == "\r" && headers.isLoaded()) {
                headersSection = false;
                contentSection = true;
            }
            
            if(headersSection) {
                if(HTTPHeader::isHeader(line))
                    headers.parseLine(line);
            }
            
            if(contentSection)
                content+=line+"\n";
        }
    }
    
    void parseHTTPStatus(istringstream& payload) {
        string version, code, description, eol;
        payload >> version;
        payload >> code;
        payload >> description;
        
        HTTPStatus _status {version, code, description};
        status = _status;
    }
    
    HTTPHeader& getHeaders(){
        return headers;
    }
    
    string toString() {
        string output;
        output += status.toString();
        output += headers.toString();
        output += "\r"; //setting up content.
        output += content;
        return output;
    }
};


