#pragma once
#include <sys/epoll.h>
#include "Epoll.h"
#include"InetAddress.h"
#include"Socket.h"
#include"functional"
#include"EventLoop.h"
#include"Connection.h"

class Epoll;
class EventLoop;

class Channel
{
private:
    // Epoll *ep_ = nullptr;    // Channel对应的红黑树，Channel与Epoll是多对一的关系，一个Channel只对应一个Epoll。
    EventLoop *loop_ = nullptr;
    int fd_=-1;              // Channel拥有的fd，Channel和fd是一对一的关系。
    uint32_t events_ = 0;        // fd_需要监视的事件。listenfd和clientfd需要监视EPOLLIN，clientfd还可能需要监视EPOLLOUT。
    uint32_t revents_=0;        // fd_已发生的事件。 
    int inepoll_=false;      // Channel是否已添加到epoll树上，如果未添加，调用epoll_ctl()的时候用EPOLL_CTL_ADD，否则用EPOLL_CTL_MOD。
    std::function<void()> readcallback_;         // fd_读事件的回调函数。
    std::function<void()> closecallback_;        // 关闭fd_的回调函数，将回调Connection::closecallback()。
    std::function<void()> errorcallback_;        // fd_发生了错误的回调函数，将回调Connection::errorcallback()。
    std::function<void()> writecallback_;        // fd_写事件的回调函数，将回调Connection::writecallback()。

public:
    Channel(EventLoop *loop,int fd);
    ~Channel();

    int fd();
    void enablereading();                       // 让epoll_wait()监视fd_的读事件，注册读事件。                   
    void disablereading();                    // 取消读事件。
    void enablewriting();                      // 注册写事件。
    void disablewriting();                     // 取消写事件。
    void setinepoll();   
    bool inepoll();
    void setrevents(uint32_t op);
    uint32_t events();
    uint32_t revents();

    void useet();

    void handleevent();

    // void newconnection(Socket *servsock);    // 处理新客户端连接请求。
    void onmessage();                                     // 处理对端发送过来的消息。

    void setreadcallback(std::function<void()> fn);    // 设置fd_读事件的回调函数。
    void setclosecallback(std::function<void()> fn);   // 设置关闭fd_的回调函数。
    void seterrorcallback(std::function<void()> fn);   // 设置fd_发生了错误的回调函数。
    void setwritecallback(std::function<void()> fn);   // 设置写事件的回调函数。
};
