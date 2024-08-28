#include"Channel.h"

Channel::Channel(Epoll* ep,int fd):ep_(ep),fd_(fd)
{

}
Channel::~Channel()
{
    //ep_和fd_都是借用，不参与他们的释放权力。
}

int Channel::fd()
{
    return fd_;
}
void Channel::enablereading()
{
    events_ = events_|EPOLLIN;
    ep_->updatechannel(this);
}
void Channel::setinepoll()
{
    inepoll_ = true;
}
bool Channel::inepoll()
{
    return inepoll_;
}
void Channel::setrevents(uint32_t op)
{
    revents_ = op;
}
uint32_t Channel::events()
{
    return events_;
}
uint32_t Channel::revents()
{
    return revents_;
}

void Channel::useet()
{
    events_ = events_|EPOLLET;
}