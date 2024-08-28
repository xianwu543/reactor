#include"Epoll.h"

Epoll::Epoll()
{
    if ((epollfd_=epoll_create(1))==-1)       // 创建epoll句柄（红黑树）。
    {
        printf("epoll_create() failed(%d).\n",errno); exit(-1);
    }
}
Epoll::~Epoll()
{
    close(epollfd_);
}

void Epoll::epoll_add(int fd,int op)
{
    struct epoll_event ev;
    ev.data.fd = fd;
    ev.events = op;

    if (epoll_ctl(epollfd_,EPOLL_CTL_ADD,fd,&ev)==-1)     // 把需要监视的fd和它的事件加入epollfd中。
    {
        printf("epoll_ctl() failed(%d).\n",errno); exit(-1);
    }
}
std::vector<struct epoll_event> Epoll::loop(int timeout)
{
    std::vector<epoll_event> evs;

    int infds=epoll_wait(epollfd_,events_,10,timeout);       // 等待监视的fd有事件发生。

    // 返回失败。
    if (infds < 0)
    {
        perror("epoll_wait() failed"); exit(-1);
    }

    // 超时。
    if (infds == 0)
    {
        printf("epoll_wait() timeout.\n"); return evs;  //返回空的vector
    }    
    //遍历到vector容器evs
    for(int ii=0;ii<infds;ii++)
    {
        evs.push_back(events_[ii]);
    }
    return evs;
}