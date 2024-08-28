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
public:
    Connection(EventLoop *loop,Socket *clientsock);
    ~Connection();
};