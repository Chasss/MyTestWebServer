#include "webserver.h"

using namespace std;

WebServer::WebServer(int port):port_(port)
{
    openLinger_=false;
	srcDir_ = getcwd(nullptr,256);
	assert(srcDir_);
    cout <<"what getcwd return(srcDir_):" << srcDir_ << endl;
    strncat(srcDir_, "/resources", 16);
    cout <<"after strnact() :"<<srcDir_ << endl;
    HttpConn::userCount = 0;
    HttpConn::srcDir = srcDir_;
    if(!InitSocket_()) 
    {
        printf("initsocket failed!\n");
        isClose_ = true;
    };
    
}

WebServer::~WebServer() 
{
    close(listenFd_);
    isClose_ = true;
    free(srcDir_);
}

bool WebServer::InitSocket_()
{
    int ret;

    struct sockaddr_in addr;
    if(port_ > 65535 || port_ < 1024)
    {
        return false;
    }

    struct linger optLinger = { 0 };
    if(openLinger_) 
    {
        /* 优雅关闭: 直到所剩数据发送完毕或超时 */
        optLinger.l_onoff = 1;
        optLinger.l_linger = 1;
    }

    listenFd_ = socket(AF_INET, SOCK_STREAM, 0);
    if(listenFd_ < 0) 
    {
        return false;
    }
    
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port_);
    ret = setsockopt(listenFd_, SOL_SOCKET, SO_LINGER, &optLinger, sizeof(optLinger));
    if(ret < 0) 
    {
        close(listenFd_);
        return false;
    }

    int optval = 1;
    /* 端口复用 */
    /* 只有最后一个套接字会正常接收数据。 */
    ret = setsockopt(listenFd_, SOL_SOCKET, SO_REUSEADDR, (const void*)&optval, sizeof(int));
    if(ret < 0) 
    {
        close(listenFd_);
        return false;
    }

    ret = bind(listenFd_, (struct sockaddr *)&addr,sizeof(addr));
    if(ret < 0)
    {
        close(listenFd_);
        return false;
    }

    ret = listen(listenFd_, 5);
    if(ret < 0) 
    {
        close(listenFd_);
        return false;
    }
    return true;
}

void WebServer::Start()
{
    struct sockaddr_in client_name;
    socklen_t client_name_len = sizeof(client_name);
    int client_sock=-1;
    //printf("http server_sock is %d\n", listenFd_);
    //printf("start accept\nport is %d\n",port_);
    while (1)
	{

	    client_sock = accept(listenFd_,
							(struct sockaddr *)&client_name,
							&client_name_len);
           
		printf("New connection....  ip: %s , port: %d\n",inet_ntoa(client_name.sin_addr),ntohs(client_name.sin_port));
        if (client_sock == -1) return;		

        //监听socket获取到已连接的用户socket，交给httpconn处理。
        users_[client_sock].init(client_sock,client_name);
        users_[client_sock].process();

	}
    close(listenFd_);
}
