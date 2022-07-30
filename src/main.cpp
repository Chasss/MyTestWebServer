/*
 * @Author       : chas
 * @Date         : 2022-07-18
 * @copyleft Apache 2.0
 */ 
#include <unistd.h>
#include "server/webserver.h"

int main() 
{
    WebServer server(1316);//port
    server.Start();
} 
  