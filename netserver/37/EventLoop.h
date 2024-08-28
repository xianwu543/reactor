#pragma once
#include <functional>
#include "Epoll.h"
#include <memory>
#include <unistd.h>
#include <sys/syscall.h>
#include <queue>
#include <mutex>
#include <sys/eventfd.h>
#include <sys/timerfd.h>      // 定时器需要包含这个头文件。
#include <map>
#include "Connection.h"
#include<atomic>

class Channel;
class Epoll;
class Connection;
using spConnection=std::shared_ptr<Connection>;

// 事件循环类。
class EventLoop
{
private:
    int  timetvl_;                                                             // 闹钟时间间隔，单位：秒。。
    int  timeout_;                                                           // Connection对象超时的时间，单位：秒。
    std::unique_ptr<Epoll> ep_;                                   // 每个事件循环只有一个Epoll。
    std::function<void(EventLoop*)> epolltimeoutcallback_;         // epoll_wait()超时的回调函数。
    pid_t threadid_;                                                       // 事件循环所在线程的id。
    std::queue<std::function<void()>> taskqueue_;    // 事件循环线程被eventfd唤醒后执行的任务队列。
    std::mutex mutex_;                                                  // 任务队列同步的互斥锁。
    int wakeupfd_;                                                         // 用于唤醒事件循环线程的eventfd。
    std::unique_ptr<Channel> wakechannel_;              // eventfd的Channel。
    int timerfd_;                                                             // 定时器的fd。
    std::unique_ptr<Channel> timerchannel_;              // 定时器的Channel。
    bool mainloop_;                                                       // true-是主事件循环，false-是从事件循环。
     std::mutex mmutex_;                                              // 保护conns_的互斥锁。
    std::map<int,spConnection> conns_;                      // 存放运行在该事件循环上全部的Connection对象。
    std::function<void(int)>  timercallback_;                 // 删除TcpServer中超时的Connection对象，将被设置为TcpServer::removeconn()
    std::atomic_bool stop_;                                            // 初始值为false，如果设置为true，表示停止事件循环。
    // 1、在事件循环中增加map<int,spConnect> conns_容器，存放运行在该事件循环上全部的Connection对象。
    // 2、如果闹钟时间到了，遍历conns_，判断每个Connection对象是否超时。
    // 3、如果超时了，从conns_中删除Connection对象；
    // 4、还需要从TcpServer.conns_中删除Connection对象。
    // 5、TcpServer和EventLoop的map容器需要加锁。
    // 6、闹钟时间间隔和超时时间参数化。
public:
    EventLoop(bool mainloop,int timetvl=30,int timeout=80);                   // 在构造函数中创建Epoll对象ep_。
    ~EventLoop();                // 在析构函数中销毁ep_。

    void run();                      // 运行事件循环。
    void stop();                // 停止事件循环。
    void wakeupstop();          //停止函数的唤醒事件循环函数。
    
    void updatechannel(Channel *ch);                        // 把channel添加/更新到红黑树上，channel中有fd，也有需要监视的事件。
    void removechannel(Channel *ch);                       // 从黑树上删除channel。
    void setepolltimeoutcallback(std::function<void(EventLoop*)> fn);  // 设置epoll_wait()超时的回调函数。

    bool isiothread();

    void queueinloop(std::function<void()> fn);
    void wakeup();
    void handlewakeup();

    void handletimer();

    void newconnection(spConnection conn);
    void settimercallback(std::function<void(int)> fn);  // 将被设置为TcpServer::removeconn()
};