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

// void Epoll::epoll_add(int fd,int op)
// {
//     struct epoll_event ev;
//     ev.data.fd = fd;
//     ev.events = op;

//     if (epoll_ctl(epollfd_,EPOLL_CTL_ADD,fd,&ev)==-1)     // 把需要监视的fd和它的事件加入epollfd中。
//     {
//         printf("epoll_ctl() failed(%d).\n",errno); exit(-1);
//     }
// }
void Epoll::updatechannel(Channel *ch)
{
    struct epoll_event ev;
    ev.data.ptr = ch;
    ev.events = ch->events();
    
    if(ch->inepoll()==false)
    {
        if (epoll_ctl(epollfd_,EPOLL_CTL_ADD,ch->fd(),&ev)==-1)     // 把需要监视的fd和它的事件加入epollfd中。
        {
            printf("epoll_ctl() failed(%d).\n",errno); exit(-1);
        }   
        ch->setinepoll();   //设置“已经在红黑树上了”
    }
    else
    {
        if (epoll_ctl(epollfd_,EPOLL_CTL_MOD,ch->fd(),&ev)==-1)     // 把需要监视的fd和它的事件加入epollfd中。
        {
            printf("epoll_ctl() failed(%d).\n",errno); exit(-1);
        }   
    }

}
void Epoll::removechannel(Channel *ch)
{
    if(ch->inepoll())
    {
        printf("removechannel()\n");
        
        if (epoll_ctl(epollfd_,EPOLL_CTL_DEL,ch->fd(),0)==-1)     // 把需要监视的fd和它的事件加入epollfd中。
        {
            printf("epoll_ctl() failed(%d).\n",errno); exit(-1);
        }   
        ch->setinepoll();   //设置“已经在红黑树上了”
    }
}
std::vector<Channel*> Epoll::loop(int timeout)
{
    std::vector<Channel*> channels;

    int infds=epoll_wait(epollfd_,events_,10,timeout);       // 等待监视的fd有事件发生。

    // 返回失败。
    if (infds < 0)
    {
        perror("epoll_wait() failed"); exit(-1);
    }

    // 超时。
    if (infds == 0)
    {
        // printf("epoll_wait() timeout.\n"); 
        return channels;  //返回空的vector
    }    
    //遍历到vector容器evs
    for(int ii=0;ii<infds;ii++)
    {
        Channel *ch = (Channel*)events_[ii].data.ptr;
        ch->setrevents(events_[ii].events);
        channels.push_back(ch);
    }
    return channels;
}