/*
 * 程序名：tcpepoll.cpp，此程序用于演示采用epoll模型实现网络通讯的服务端。
 * 作者：张咸武
*/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>          
#include <arpa/inet.h>
#include <sys/fcntl.h>
#include <sys/epoll.h>
#include <netinet/tcp.h>      // TCP_NODELAY需要包含这个头文件。
#include"InetAddress.h"
#include"Socket.h"
#include"Epoll.h"
#include"Channel.h"

int main(int argc,char *argv[])
{
    if (argc != 3) 
    { 
        printf("usage: ./tcpepoll ip port\n"); 
        printf("example: ./tcpepoll 192.168.192.136 5085\n\n"); 
        return -1; 
    }

    Socket servsock(createnonblocking());

    servsock.setreuseport(1);
    servsock.setkeepalive(1);
    servsock.setreuseaddr(1);
    servsock.settcpnodelay(1);

    InetAddress servaddr(argv[1],atoi(argv[2]));

    servsock.bind(servaddr);
    servsock.listen(128);

    Epoll ep;

    Channel *servchannel = new Channel(&ep,servsock.fd());
    servchannel->setreadcallback(std::bind(&Channel::newconnection,servchannel,&servsock));
    servchannel->enablereading();

    std::vector<Channel*> channels;      // 存放epoll_wait()返回事件的数组。

    while (true)        // 事件循环。
    {
        channels = ep.loop();

        if(channels.size()==0)   //说明超时
        {
            //超时处理
            continue;
        }
        
        // 如果infds>0，表示有事件发生的fd的数量。
        for (auto &ch:channels)       // 遍历epoll返回的数组evs。
        {
            ch->handleevent();
        }
    }

  return 0;
}