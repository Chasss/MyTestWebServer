# MyTestWebServer

## 简介

基于C++实现的轻量级HTTP服务器。

内容主要参考[tinyhttpd](http://tinyhttpd.sourceforge.net/)与[MyPoorWebServer](https://github.com/forthespada/MyPoorWebServer)。

其实一开始看的是比较火的[tinyWebServer](https://github.com/qinguoyi/TinyWebServer)，但一开始就去实现这些内容感觉比较难，比如用半同步半反应堆模式将逻辑与IO分离，即使看明白了但直接从零组织起来还是感觉不太容易。

所以打算想从比较简单易懂的开始作为基础，然后往上添加扩展功能，tinyhttpd成为了我的目标，可以理解为这是对tinyhttpd的C++面向对象再实现，c++代码架构与类设计参考了[Webserver](https://github.com/markparticle/WebServer)。

也就这是我的webserver1.0版，待扩展。


## 主要实现功能

* 实现GET/POST两种请求的解析与响应，使用cgi脚本响应POST请求
* 利用多线程为每个socket连接提供服务。
* 利用双管道实现cgi进程与服务器进程间的通信与数据交换。

## 环境需求

* Linux
* C++11
* python3

## 目录树
```
.
├── bin
│   └── server
├── build
│   └── makefile
├── resources
│   ├── post.cgi
│   ├── post.html
│   └── test.html
├── src
│   ├── http
│   │   ├── httpconn.cpp
│   │   ├── httpconn.h
│   │   ├── httprequest.cpp
│   │   ├── httprequest.h
│   │   ├── httpresponse.cpp
│   │   └── httpresponse.h
│   ├── server
│   │   ├── webserver.cpp
│   │   └── webserver.h
│   └── main.cpp
├── makefile
└── README.md
```

## 编译及运行
* 修改文件权限

```C++
cd resource
sudo chmod 600 test.html
sudo chmod 600 post.html
sudo chmod +X post.cgi
```
可以直接测试一下cgi文件能否正常执行，注意下第一行的python解释器路径，因为我自己的是python3

* 编译运行
```C++
make
./bin/server
```

## 一些学习过程

* 牢固了一些操作系统、计算机网络、linux进程线程通信/socket通信的知识。
* 对于阻塞IO，如果缓冲区为中没有足够字节的数据，IO会一直阻塞，所以要确定要要读写哪些数据，什么时候结束，不要在缓冲区中没有足够的数据还去IO，这样就会就造成阻塞。例如在本项目的读客户端socket中，因为GET/POST报文格式开始比较确定的，对于GET请求就是读到空行结束，对于POST是读取到空行后Content-Length长度的body结束。
* 在tinyhttpd的基础上，学习熟悉了一些C++面向对象开发，例如我在项目中将报文解析和对客户端socket的读都封装进HttpRequest类中，将报文响应和对客户端socket的写都封装进HttpResponse类中。
* 准备在这个项目的基础上添加更多功能。

