#pragma once
#include "EventLoop.h"
#include "Socket.h"
#include "Channel.h"
#include"Acceptor.h"
#include"Connection.h"
#include<map>

class TcpServer
{
private:
    EventLoop loop_;
    Acceptor *acceptor_;   // 一个TcpServer只有一个Acceptor对象。
    std::map<int,Connection*> conns_;           // 一个TcpServer有多个Connection对象，存放在map容器中。
public:
    TcpServer(const std::string &ip,uint16_t port);
    ~TcpServer();

    void start();
    void newconnection(Socket *clientsock);
};