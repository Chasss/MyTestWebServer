#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>
#include <regex>
#include <errno.h>
#include <sys/socket.h>
#include <algorithm>
class HttpRequest
{
public:
    /*
     enum PARSE_STATE 
     {
        REQUEST_LINE,
        HEADERS,
        BODY,
        FINISH,        
    };

    enum HTTP_CODE 
    {
        NO_REQUEST = 0,
        GET_REQUEST,
        BAD_REQUEST,
        NO_RESOURSE,
        FORBIDDENT_REQUEST,
        FILE_REQUEST,
        INTERNAL_ERROR,
        CLOSED_CONNECTION,
    };
*/
    HttpRequest() { Init(); }
    ~HttpRequest() = default;
    void Init();
    bool parse(int client_fd);

    std::string method() const;
    std::string version() const;
    std::string param() const;
    std::string path() const;
    std::string &path();
    std::string body() const;

private:
    /*
    bool ParseRequestLine_(const std::string& line);
    void ParseHeader_(const std::string& line);
    void ParseBody_(const std::string& line);

    void ParsePath_();
    void ParsePost_();
    void ParseFromUrlencoded_();
*/
    int get_line(int sock, std::vector<char> &buf, int size);

    //PARSE_STATE state_;
    std::string method_, path_, param_, version_, body_;
    std::unordered_map<std::string, std::string> header_;
    //std::unordered_map<std::string, std::string> post_;

    //static const std::unordered_set<std::string> DEFAULT_HTML;
    //static const std::unordered_map<std::string, int> DEFAULT_HTML_TAG;
    //static int ConverHex(char ch);
};

#endif //HTTP_REQUEST_H