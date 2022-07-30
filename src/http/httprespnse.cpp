#include "httpresponse.h"

using namespace std;

HttpResponse::HttpResponse() {
    path_ = srcDir_=method_=body_= "";
    mmFile_ = nullptr; 
    mmFileStat_ = { 0 };
    code_=-1;
};

HttpResponse::~HttpResponse() {
	
}


void HttpResponse::Init(int clientfd,const string& srcDir, string& path, const string& method,const string& body,const string& params,int code)
{
	client = clientfd;
    srcDir_=srcDir;
    path_=path;
    method_=method;
    body_=body;
    mmFile_ = nullptr; 
    mmFileStat_ = { 0 };
    params_=params;
    code_=code;
}

void HttpResponse::serve_file(string filename)
{
	printf("server_file: %s\n",filename.c_str());
    FILE *resource = NULL;	
    char buf[1024];
	//打开文件
	resource = fopen(filename.c_str(), "r");
	if (resource == NULL)
        not_found();
	else
	{
        headers();
        fgets(buf, sizeof(buf), resource);
        while (!feof(resource))
        {
            send(client, buf, strlen(buf), 0);
            fgets(buf, sizeof(buf), resource);
        }
    }
    fclose(resource);//关闭文件句柄
}
void HttpResponse::execute_cgi()
{
     char buf[1024];
	 int cgi_output[2];
	 int cgi_input[2];
	 
	 pid_t pid;
	 int status;

	 int i;
	 char c;

	//int numchars = 1;
    sprintf(buf, "HTTP/1.0 200 OK\r\n");
	send(client, buf, strlen(buf), 0);
	if (pipe(cgi_output) < 0)
    {
		cannot_execute(); 
        return;
    }
	if (pipe(cgi_input) < 0) 
    { 
		cannot_execute();
        return;
    }

	if ( (pid = fork()) < 0 ) 
    {
		cannot_execute();
        return;
	}
    if (pid == 0)  /* 子进程: 运行CGI 脚本 */
	{
		char meth_env[255];
		char query_env[255];
		char length_env[255];

		dup2(cgi_output[1], 1);
		dup2(cgi_input[0], 0);


		close(cgi_output[0]);//关闭了cgi_output中的读通道
		close(cgi_input[1]);//关闭了cgi_input中的写通道


		sprintf(meth_env, "REQUEST_METHOD=%s", method_.c_str());
		putenv(meth_env);

		if (method_=="GET") 
        {
            sprintf(query_env, "QUERY_STRING=%s", params_.c_str());		  //存储QUERY_STRING
            putenv(query_env);
        }
        else 
        {   /* POST */
			//存储CONTENT_LENGTH
		    sprintf(length_env, "CONTENT_LENGTH=%d", body_.size());
		    putenv(length_env);
		}
		string filename=srcDir_+path_;
	    execl(filename.c_str(), filename.c_str(), NULL);//执行CGI脚本
		exit(0);
	 } 
	 else 
     {  
        close(cgi_output[1]);
        close(cgi_input[0]);
        if (method_=="POST")
        {
		    for (i = 0; i < body_.size(); i++) 
			{
				write(cgi_input[1], &body_[i], 1);
			}
        }



		//读取cgi脚本返回数据

		while (read(cgi_output[0], &c, 1) > 0)
			//发送给浏览器
		{			
		    send(client, &c, 1, 0);
		}

		//运行结束关闭
		close(cgi_output[0]);
		close(cgi_input[1]);


		waitpid(pid, &status, 0);
	}
}
void HttpResponse::headers()
{
    char buf[1024];
	//发送HTTP头
	strcpy(buf, "HTTP/1.0 200 OK\r\n");
	send(client, buf, strlen(buf), 0);
	strcpy(buf, SERVER_STRING.c_str());
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "Content-Type: text/html\r\n");
	send(client, buf, strlen(buf), 0);
	strcpy(buf, "\r\n");
	send(client, buf, strlen(buf), 0);
}
void HttpResponse::not_found()
{
    char buf[1024];
	sprintf(buf, "HTTP/1.0 404 NOT FOUND\r\n");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, SERVER_STRING.c_str());
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "Content-Type: text/html\r\n");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "\r\n");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "<HTML><TITLE>Not Found</TITLE>\r\n");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "<BODY><P>The server could not fulfill\r\n");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "your request because the resource specified\r\n");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "is unavailable or nonexistent.\r\n");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "</BODY></HTML>\r\n");
	send(client, buf, strlen(buf), 0);
}
void HttpResponse::bad_request()
{
    char buf[1024];
	//发送400
	sprintf(buf, "HTTP/1.0 400 BAD REQUEST\r\n");
	send(client, buf, sizeof(buf), 0);
	sprintf(buf, "Content-type: text/html\r\n");
	send(client, buf, sizeof(buf), 0);
	sprintf(buf, "\r\n");
	send(client, buf, sizeof(buf), 0);
	sprintf(buf, "<P>Your browser sent a bad request, ");
	send(client, buf, sizeof(buf), 0);
	sprintf(buf, "such as a POST without a Content-Length.\r\n");
	send(client, buf, sizeof(buf), 0);
}
void HttpResponse::cannot_execute()
{
	char buf[1024];
	//发送500
	 sprintf(buf, "HTTP/1.0 500 Internal Server Error\r\n");
	 send(client, buf, strlen(buf), 0);
	 sprintf(buf, "Content-type: text/html\r\n");
	 send(client, buf, strlen(buf), 0);
	 sprintf(buf, "\r\n");
	 send(client, buf, strlen(buf), 0);
	 sprintf(buf, "<P>Error prohibited CGI execution.\r\n");
	 send(client, buf, strlen(buf), 0);
}
void HttpResponse::MakeResponse()
{
    if(stat((srcDir_ + path_).data(), &mmFileStat_) < 0 || S_ISDIR(mmFileStat_.st_mode)) 
    {
        code_ = 404;
        not_found();
        return;
    }

    if(method_=="GET" && params_=="")
    {
        serve_file(srcDir_+path_);
    }
    else
    {
        execute_cgi();
    }
}


