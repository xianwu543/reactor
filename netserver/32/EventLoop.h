#pragma once
#include <functional>
#include "Epoll.h"
#include <memory>
#include <unistd.h>
#include <sys/syscall.h>
#include <queue>
#include <mutex>
#include <sys/eventfd.h>

class Channel;
class Epoll;

// 事件循环类。
class EventLoop
{
private:
    std::unique_ptr<Epoll> ep_;                       // 每个事件循环只有一个Epoll。
    std::function<void(EventLoop*)> epolltimeoutcallback_;         // epoll_wait()超时的回调函数。
    pid_t threadid_;
    std::queue<std::function<void()>> taskqueue_;    // 事件循环线程被eventfd唤醒后执行的任务队列。
    std::mutex mutex_;                                                  // 任务队列同步的互斥锁。
    int wakeupfd_;                                                         // 用于唤醒事件循环线程的eventfd。
    std::unique_ptr<Channel> wakechannel_;              // eventfd的Channel。
public:
    EventLoop();                   // 在构造函数中创建Epoll对象ep_。
    ~EventLoop();                // 在析构函数中销毁ep_。

    void run();                      // 运行事件循环。

    void updatechannel(Channel *ch);                        // 把channel添加/更新到红黑树上，channel中有fd，也有需要监视的事件。
    void removechannel(Channel *ch);                       // 从黑树上删除channel。
    void setepolltimeoutcallback(std::function<void(EventLoop*)> fn);  // 设置epoll_wait()超时的回调函数。

    bool isiothread();

    void wakeup(std::function<void()> fn);
    void handlewakeup();

};