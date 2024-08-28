#include"EventLoop.h"

EventLoop::EventLoop():ep_(new Epoll)
{

}
EventLoop::~EventLoop()
{
    delete ep_; //ep_是属于EventLoop创建和释放。
}

void EventLoop::run()
{
    while (true)        // 事件循环。
    {
        std::vector<Channel*> channels;      // 存放epoll_wait()返回事件的数组。
        channels = ep_->loop();

        if(channels.size()==0)   //说明超时
        {
            //超时处理
            continue;
        }
        
        // 如果infds>0，表示有事件发生的fd的数量。
        for (auto &ch:channels)       // 遍历epoll返回的数组evs。
        {
            ch->handleevent();
        }
    }
}
Epoll *EventLoop::ep()
{
    return ep_;
}