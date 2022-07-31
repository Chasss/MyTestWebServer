#ifndef HTTP_CONN_H
#define HTTP_CONN_H

#include <sys/types.h>
#include <sys/uio.h>   // readv/writev
#include <arpa/inet.h> // sockaddr_in
#include <stdlib.h>    // atoi()
#include <errno.h>
#include <assert.h>
#include <atomic>
#include <pthread.h>

#include "httprequest.h"
#include "httpresponse.h"
class HttpConn
{
public:
    HttpConn();

    ~HttpConn();

    void init(int sockFd, const sockaddr_in &addr);

    bool process();

    void Close();

    int GetFd() const;

    int GetPort() const;

    const char *GetIP() const;

    sockaddr_in GetAddr() const;

    static const char *srcDir;
    static std::atomic<int> userCount;

private:
    static void *thread_process(void *from_client);
    int fd_;
    struct sockaddr_in addr_;
    bool isClose_;
    HttpRequest request_;
    HttpResponse response_;
};

#endif //HTTP_CONN_H