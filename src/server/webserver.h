/*
 * @Author       : chas
 * @Date         : 2022-06-17
 * @copyleft Apache 2.0
 */
#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <unordered_map>
#include <fcntl.h>  // fcntl()
#include <unistd.h> // close()
#include <assert.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <iostream>

#include "../http/httpconn.h"
class WebServer
{
public:
    WebServer(int port);

    ~WebServer();
    void Start();

private:
    bool InitSocket_();
    static const int MAX_FD = 65536;

    int port_;
    bool isClose_;
    int listenFd_;    //监听socket
    bool openLinger_; //SO_LINGER选项
    char *srcDir_;
    std::unordered_map<int, HttpConn> users_;
};

#endif //WEBSERVER_H