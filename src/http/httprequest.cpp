#include "httprequest.h"
#include <iostream>
using namespace std;

void HttpRequest::Init()
{
    method_ = path_ = version_ = body_ = param_ = "";
    //state_ = REQUEST_LINE;
    header_.clear();
    //post_.clear();
}

int HttpRequest::get_line(int sock, vector<char> &buf, int size)
{
    int i = 0;
    char c = '\0';
    int n;

    while ((i < size - 1) && (c != '\n'))
    {
        n = recv(sock, &c, 1, 0);

        if (n > 0)
        {
            if (c == '\r')
            {
                n = recv(sock, &c, 1, MSG_PEEK);
                if ((n > 0) && (c == '\n'))
                {
                    recv(sock, &c, 1, 0);
                }
                else
                {
                    c = '\n';
                }
            }
            buf[i] = c;
            ++i;
        }
        else
        {
            c = '\n';
        }
    }
    buf[i] = '\0';
    return (i);
}

bool HttpRequest::parse(int client_fd)
{
    vector<char> buf(1024, 0);
    size_t i = 0, j = 0;

    int numchars = 0;
    numchars = get_line(client_fd, buf, buf.size());
    //printf("numchars = %d\n",numchars);
    while (!isspace(buf[j]) && i < buf.size())
    {
        //提取其中的请求方式
        //method_[i] = buf[j];
        ++i;
        ++j;
    }
    method_ = string(buf.begin(), buf.begin() + i);
    if (method_ != "GET" && method_ != "POST")
    {
        return false;
    }

    //跳过空格
    while (isspace(buf[j]) && j < buf.size())
    {
        ++j;
        ++i;
    }
    //找到url结尾
    while (!isspace(buf[j]) && j < buf.size())
    {
        //++i;
        ++j;
    }
    path_ = string(string(buf.begin() + i, buf.begin() + j));
    //截取查询参数
    if (method_ == "GET")
    {
        size_t pos = path_.find("?");
        if (pos != path_.npos)
        {
            param_ = path_.substr(pos + 1);
            path_ = path_.substr(0, pos);
        }
    }
    //跳过空格
    i = j;
    while (isspace(buf[j]) && j < buf.size())
    {
        ++j;
        ++i;
    }
    //获取版本号
    while (buf[j] != '\r' && j < buf.size())
    {
        //++i;
        ++j;
    }
    version_ = string(buf.begin() + i, buf.begin() + j);

    //获取header line
    numchars = get_line(client_fd, buf, buf.size());
    while ((numchars > 0) && find(buf.begin(), buf.begin() + numchars, '\n') != buf.begin())
    {
        numchars = get_line(client_fd, buf, buf.size());
        string temp(buf.begin(), buf.begin() + numchars);
        if (temp.find("Content-Length:") != temp.npos)
        {
            header_["Content-Length"] = temp.substr(sizeof("Content-Length:"));
        }
    }
    //获取body_
    if (method_ == "POST")
    {
        char c = 0;
        size_t content_length = stoi(header_["Content-Length"]);
        for (i = 0; i < content_length; ++i)
        {
            recv(client_fd, &c, 1, 0);
            body_ += c;
        }
    }
    //清空读缓冲区
    /*
    numchars = get_line(client_fd, buf, sizeof(buf));
    cout<<"numchars = "<<numchars<<endl;
	while ((numchars > 0) && find(buf.begin(),bef.begin(),'\n')!=bef.begin()) 
	{
        numchars = get_line(client_fd, buf, sizeof(buf));
        cout<<"numchars = "<<numchars<<endl;
	}
    */
    return true;
}

std::string HttpRequest::path() const
{
    return path_;
}

std::string &HttpRequest::path()
{
    return path_;
}
std::string HttpRequest::method() const
{
    return method_;
}

std::string HttpRequest::version() const
{
    return version_;
}

std::string HttpRequest::body() const
{
    return body_;
}

std::string HttpRequest::param() const
{
    return param_;
}