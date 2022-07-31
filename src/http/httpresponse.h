#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H

#include <unordered_map>
#include <fcntl.h>    // open
#include <unistd.h>   // close
#include <sys/stat.h> // stat
#include <sys/mman.h> // mmap, munmap
#include <sys/socket.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string>

class HttpResponse
{
public:
    HttpResponse();
    ~HttpResponse();

    void Init(int clientfd, const std::string &srcDir, std::string &path, const std::string &method, const std::string &body, const std::string &params, int code);
    void MakeResponse();

private:
    void serve_file(std::string filename);
    void execute_cgi();
    void headers();
    void not_found();
    void bad_request();
    void cannot_execute();
    std::string path_;
    std::string srcDir_;
    std::string method_;
    std::string body_;
    std::string params_;
    char *mmFile_;
    struct stat mmFileStat_;
    const std::string SERVER_STRING = "Server: Chas's http/0.1.0\r\n"; //定义个人server名称;
    int code_;
    int client;
};

#endif //HTTP_RESPONSE_H