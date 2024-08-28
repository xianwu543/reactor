/*
 * 程序名：tcpepoll.cpp，此程序用于演示采用epoll模型实现网络通讯的服务端。
 * 作者：张咸武
*/
#include"EchoServer.h"

int main(int argc,char *argv[])
{
    if (argc != 3) 
    { 
        printf("usage: ./tcpepoll ip port\n"); 
        printf("example: ./tcpepoll 192.168.192.136 5085\n\n"); 
        return -1; 
    }

  EchoServer echoserver(argv[1],atoi(argv[2]));

  echoserver.Start();

  return 0;
}