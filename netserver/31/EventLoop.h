#pragma once
#include <functional>
#include "Epoll.h"
#include <memory>

class Channel;
class Epoll;

// 事件循环类。
class EventLoop
{
private:
    std::unique_ptr<Epoll> ep_;                       // 每个事件循环只有一个Epoll。
    std::function<void(EventLoop*)> epolltimeoutcallback_;         // epoll_wait()超时的回调函数。
public:
    EventLoop();                   // 在构造函数中创建Epoll对象ep_。
    ~EventLoop();                // 在析构函数中销毁ep_。

    void run();                      // 运行事件循环。

    void updatechannel(Channel *ch);                        // 把channel添加/更新到红黑树上，channel中有fd，也有需要监视的事件。
    void removechannel(Channel *ch);                       // 从黑树上删除channel。
    void setepolltimeoutcallback(std::function<void(EventLoop*)> fn);  // 设置epoll_wait()超时的回调函数。
};