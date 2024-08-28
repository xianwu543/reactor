#include"EventLoop.h"

EventLoop::EventLoop():ep_(new Epoll)
{

}
EventLoop::~EventLoop()
{
    delete ep_; //ep_是属于EventLoop创建和释放。
}

#include <unistd.h>
#include <sys/syscall.h>   /* For SYS_xxx definitions */
void EventLoop::run()
{
// printf("线程id:%d\n",syscall(SYS_gettid));
    while (true)        // 事件循环。
    {
        std::vector<Channel*> channels;      // 存放epoll_wait()返回事件的数组。
        channels = ep_->loop(10*1000);

        if(channels.size()==0)   //说明超时
        {
            //超时处理
            epolltimeoutcallback_(this);
        }
        else
        {
            // 如果infds>0，表示有事件发生的fd的数量。
            for (auto &ch:channels)       // 遍历epoll返回的数组evs。
            {
                ch->handleevent();
            }
        }
    }
}

void EventLoop::updatechannel(Channel *ch)
{
    ep_->updatechannel(ch);
}

// 设置epoll_wait()超时的回调函数。
void EventLoop::setepolltimeoutcallback(std::function<void(EventLoop*)> fn)
{
    epolltimeoutcallback_ = fn;
}