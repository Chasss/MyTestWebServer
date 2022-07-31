#include "httpconn.h"
using namespace std;

const char *HttpConn::srcDir;
std::atomic<int> HttpConn::userCount;

HttpConn::HttpConn()
{
    fd_ = -1;
    addr_ = {0};
    isClose_ = true;
};
HttpConn::~HttpConn()
{
    Close();
};

void HttpConn::init(int fd, const sockaddr_in &addr)
{
    assert(fd > 0);
    userCount++;
    addr_ = addr;
    fd_ = fd;
    isClose_ = false;
}

void HttpConn::Close()
{
    //response_.UnmapFile();
    if (isClose_ == false)
    {
        isClose_ = true;
        --userCount;
        close(fd_);
    }
}

int HttpConn::GetFd() const
{
    return fd_;
};

struct sockaddr_in HttpConn::GetAddr() const
{
    return addr_;
}

const char *HttpConn::GetIP() const
{
    return inet_ntoa(addr_.sin_addr);
}

int HttpConn::GetPort() const
{
    return addr_.sin_port;
}

void *HttpConn::thread_process(void *httpconn)
{
    HttpConn *conn = (HttpConn *)httpconn;
    conn->request_.Init();
    //printf("start parse\n");
    if (conn->request_.parse(conn->fd_))
    {
        //成功解析请求,开始响应
        conn->response_.Init(conn->fd_, conn->srcDir, conn->request_.path(), conn->request_.method(), conn->request_.body(), conn->request_.param(), 200);
        conn->response_.MakeResponse();
    }
    conn->Close();
    return nullptr;
}

bool HttpConn::process()
{
    pthread_t newthread;
    if (pthread_create(&newthread, NULL, thread_process, (void *)this) != 0)
    {
        return false;
    }
    pthread_detach(newthread);
    return true;
}