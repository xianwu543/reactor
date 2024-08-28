#pragma once
#include <functional>
#include "Socket.h"
#include "InetAddress.h"
#include "Channel.h"
#include "EventLoop.h"

class EventLoop;
class Channel;

class Connection
{
private:
    EventLoop *loop_;
    Socket *clientsock_;
    Channel *clientchannel_;

    std::string ip_;               // 如果是listenfd，存放服务端监听的ip，如果是客户端连接的fd，存放对端的ip。
    uint16_t port_;              // 如果是listenfd，存放服务端监听的port，如果是客户端连接的fd，存放外部端口。

public:
    Connection(EventLoop *loop,Socket *clientsock);
    ~Connection();

    int fd() const;
    std::string ip() const;
    uint16_t port() const;
};