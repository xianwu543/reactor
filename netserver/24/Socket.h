#pragma once
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include "InetAddress.h"

// 创建一个非阻塞的socket。
int createnonblocking();

class Socket
{
private:
    int fd_;
    std::string ip_;               // 如果是listenfd，存放服务端监听的ip，如果是客户端连接的fd，存放对端的ip。
    uint16_t port_;              // 如果是listenfd，存放服务端监听的port，如果是客户端连接的fd，存放外部端口。

public:
    Socket(int fd);
    ~Socket();

    void setreuseaddr(bool on);       // 设置SO_REUSEADDR选项，true-打开，false-关闭。
    void setreuseport(bool on);       // 设置SO_REUSEPORT选项。
    void settcpnodelay(bool on);     // 设置TCP_NODELAY选项。
    void setkeepalive(bool on);       // 设置SO_KEEPALIVE选项。

    void bind(InetAddress &addr);
    void listen(int nn=128);
    int accept(InetAddress &clienyaddr);

    int fd() const;

    std::string ip() const;
    uint16_t port() const;
    void setipport(const std::string &ip,uint16_t port);
};