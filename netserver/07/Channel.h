#pragma once
#include <sys/epoll.h>
#include "Epoll.h"

class Epoll;

class Channel
{
private:
    Epoll *ep_ = nullptr;    // Channel对应的红黑树，Channel与Epoll是多对一的关系，一个Channel只对应一个Epoll。
    int fd_=-1;              // Channel拥有的fd，Channel和fd是一对一的关系。
    uint32_t events_ = 0;        // fd_需要监视的事件。listenfd和clientfd需要监视EPOLLIN，clientfd还可能需要监视EPOLLOUT。
    uint32_t revents_=0;        // fd_已发生的事件。 
    int inepoll_=false;      // Channel是否已添加到epoll树上，如果未添加，调用epoll_ctl()的时候用EPOLL_CTL_ADD，否则用EPOLL_CTL_MOD。
public:
    Channel(Epoll* ep,int fd);
    ~Channel();

    int fd();
    void enablereading();
    void setinepoll();
    bool inepoll();
    void setrevents(uint32_t op);
    uint32_t events();
    uint32_t revents();

    void useet();
};
