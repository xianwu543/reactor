#pragma once
#include"Epoll.h"

class Channel;
class Epoll;

class EventLoop
{
private:
    Epoll *ep_;
    std::function<void(EventLoop*)> epolltimeoutcallback_;         // epoll_wait()超时的回调函数。
public:
    EventLoop();
    ~EventLoop();

    void run();
    void updatechannel(Channel *ch);
    void removechannel(Channel *ch);
    void setepolltimeoutcallback(std::function<void(EventLoop*)> fn);  // 设置epoll_wait()超时的回调函数。
};  