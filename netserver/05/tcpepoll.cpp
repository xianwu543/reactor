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

    // int epollfd=epoll_create(1);        // 创建epoll句柄（红黑树）。

    // 为服务端的listenfd准备读事件。
    // struct epoll_event ev;              // 声明事件的数据结构。
    // ev.data.fd=servsock.fd();                 // 指定事件的自定义数据，会随着epoll_wait()返回的事件一并返回。
    // ev.events=EPOLLIN;                // 让epoll监视listenfd的读事件，采用水平触发。

    // epoll_ctl(epollfd,EPOLL_CTL_ADD,servsock.fd(),&ev);     // 把需要监视的listenfd和它的事件加入epollfd中。

    Epoll ep;
    ep.epoll_add(servsock.fd(),EPOLLIN);

    std::vector<epoll_event> evs;      // 存放epoll_wait()返回事件的数组。

    while (true)        // 事件循环。
    {
        // int infds=epoll_wait(epollfd,evs,10,-1);       // 等待监视的fd有事件发生。

        // // 返回失败。
        // if (infds < 0)
        // {
        //     perror("epoll_wait() failed"); break;
        // }

        // // 超时。
        // if (infds == 0)
        // {
        //     printf("epoll_wait() timeout.\n"); continue;
        // }
        
        evs = ep.loop();

        if(evs.size()==0)   //说明超时
        {
            //超时处理
            continue;
        }
        
        // 如果infds>0，表示有事件发生的fd的数量。
        for (auto &ev:evs)       // 遍历epoll返回的数组evs。
        {
            ////////////////////////////////////////////////////////////////////////
            if (ev.events & EPOLLRDHUP)                     // 对方已关闭，有些系统检测不到，可以使用EPOLLIN，recv()返回0。
            {
                printf("client(eventfd=%d) disconnected.\n",ev.data.fd);
                close(ev.data.fd);            // 关闭客户端的fd。
            }                                //  普通数据  带外数据
            else if (ev.events & (EPOLLIN|EPOLLPRI))   // 接收缓冲区中有数据可以读。
            {

                if (ev.data.fd==servsock.fd())   // 如果是listenfd有事件，表示有新的客户端连上来。
                {
                    ////////////////////////////////////////////////////////////////////////
                    InetAddress clientaddr;
                    Socket* clientsock = new Socket(servsock.accept(clientaddr));

                    printf ("accept client(fd=%d,ip=%s,port=%d) ok.\n",clientsock->fd(),clientaddr.ip(),clientaddr.port());

                    // 为新客户端连接准备读事件，并添加到epoll中。
                    // ev.data.fd=clientsock->fd();
                    // ev.events=EPOLLIN|EPOLLET;           // 边缘触发。
                    // epoll_ctl(epollfd,EPOLL_CTL_ADD,clientsock->fd(),&ev);
                    ep.epoll_add(clientsock->fd(),EPOLLIN|EPOLLET);
                    ////////////////////////////////////////////////////////////////////////
                }
                else
                {
                    char buffer[1024];
                    while (true)             // 由于使用非阻塞IO，一次读取buffer大小数据，直到全部的数据读取完毕。
                    {    
                        bzero(&buffer, sizeof(buffer));
                        ssize_t nread = read(ev.data.fd, buffer, sizeof(buffer));    // 这行代码用了read()，也可以用recv()，一样的，不要纠结。
                        if (nread > 0)      // 成功的读取到了数据。
                        {
                            // 把接收到的报文内容原封不动的发回去。
                            printf("recv(eventfd=%d):%s\n",ev.data.fd,buffer);
                            send(ev.data.fd,buffer,strlen(buffer),0);
                        } 
                        else if (nread == -1 && errno == EINTR) // 读取数据的时候被信号中断，继续读取。
                        {  
                            continue;
                        } 
                        else if (nread == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))) // 全部的数据已读取完毕。
                        {
                            break;
                        } 
                        else if (nread == 0)  // 客户端连接已断开。
                        {  
                            printf("client(eventfd=%d) disconnected.\n",ev.data.fd);
                            close(ev.data.fd);            // 关闭客户端的fd。
                            break;
                        }
                    }
                }
            }
            else if (ev.events & EPOLLOUT)                  // 有数据需要写，暂时没有代码，以后再说。
            {
            }
            else                                                                    // 其它事件，都视为错误。
            {
                printf("client(eventfd=%d) error.\n",ev.data.fd);
                close(ev.data.fd);            // 关闭客户端的fd。
            }
            ////////////////////////////////////////////////////////////////////////
        }
    }

  return 0;
}