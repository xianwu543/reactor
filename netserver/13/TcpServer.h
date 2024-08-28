#pragma once
#include "EventLoop.h"
#include "Socket.h"
#include "Channel.h"
#include"Acceptor.h"

class TcpServer
{
private:
    EventLoop loop_;
    Acceptor *acceptor_;   // 一个TcpServer只有一个Acceptor对象。
public:
    TcpServer(const std::string &ip,uint16_t port);
    ~TcpServer();

    void start();
};