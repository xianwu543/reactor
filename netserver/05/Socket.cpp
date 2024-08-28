#include"Socket.h"

// 创建一个非阻塞的socket。
int createnonblocking()
{
    int listenfd = socket(AF_INET,SOCK_STREAM|SOCK_NONBLOCK,IPPROTO_TCP);
    if (listenfd < 0)
    {
        perror("socket() failed"); return -1;
    }
    return listenfd;
}
Socket::Socket(int fd):fd_(fd)
{
    
}
Socket::~Socket()
{
    ::close(fd_);
}

void Socket::setreuseaddr(bool on)       // 设置SO_REUSEADDR选项，true-打开，false-关闭。
{
    int optval = on ? 1 : 0;
    setsockopt(fd_,SOL_SOCKET,SO_REUSEADDR,&optval,static_cast<socklen_t>(sizeof optval));    // 必须的。
}
void Socket::setreuseport(bool on)       // 设置SO_REUSEPORT选项。
{
    int optval = on ? 1 : 0;
    setsockopt(fd_,SOL_SOCKET,TCP_NODELAY   ,&optval,static_cast<socklen_t>(sizeof optval));    // 必须的。
}
void Socket::settcpnodelay(bool on)     // 设置TCP_NODELAY选项。
{
    int optval = on ? 1 : 0;
    setsockopt(fd_,SOL_SOCKET,SO_REUSEPORT ,&optval,static_cast<socklen_t>(sizeof optval));    // 有用，但是，在Reactor中意义不大。
}
void Socket::setkeepalive(bool on)       // 设置SO_KEEPALIVE选项。
{
    int optval = on ? 1 : 0;
    setsockopt(fd_,SOL_SOCKET,SO_KEEPALIVE   ,&optval,static_cast<socklen_t>(sizeof optval));    // 可能有用，但是，建议自己做心跳。
}

void Socket::bind(InetAddress &addr)
{
    if (::bind(fd_,addr.addr(),sizeof(struct sockaddr)) < 0 )
    {
        perror("bind() failed"); close(fd_); exit(-1);
    }    
}
void Socket::listen(int nn)
{
    if (::listen(fd_,nn) != 0 )        // 在高并发的网络服务器中，第二个参数要大一些。
    {
        perror("listen() failed"); close(fd_); exit(-1);
    }
}
int Socket::accept(InetAddress &clientaddr)
{
    struct sockaddr_in peer;
    socklen_t len = sizeof(peer);
    int clientfd = accept4(fd_,(struct sockaddr*)&peer,&len,SOCK_NONBLOCK);
    // setnonblocking(clientfd);         // 客户端连接的fd必须设置为非阻塞的。
    clientaddr.setaddr(peer);

    return clientfd;
}

int Socket::fd() const
{
    return fd_;
}