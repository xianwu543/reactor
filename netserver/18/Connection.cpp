#include"Connection.h"

Connection::Connection(EventLoop *loop,Socket *clientsock):loop_(loop),clientsock_(clientsock)
{
    clientchannel_ = new Channel(loop_,clientsock_->fd());
    clientchannel_->setreadcallback(std::bind(&Channel::onmessage,clientchannel_));
    clientchannel_->setclosecallback(std::bind(&Connection::closecallback,this));
    clientchannel_->seterrorcallback(std::bind(&Connection::errorcallback,this));
    clientchannel_->useet();
    clientchannel_->enablereading();
}

Connection::~Connection()
{
    delete clientchannel_;
    delete clientsock_;     // 是外部传入的，但是他的生命周期与Connection相同。
}

int Connection::fd() const
{
    return clientsock_->fd();
}
std::string Connection::ip() const
{
    return clientsock_->ip();
}
uint16_t Connection::port() const
{
    return clientsock_->port();
}

// TCP连接关闭（断开）的回调函数，供Channel回调。
void Connection::closecallback()
{
    closecallback_(this);
}

// TCP连接错误的回调函数，供Channel回调。
void Connection::errorcallback()
{
    errorcallback_(this);
}

// 设置关闭fd_的回调函数。
void Connection::setclosecallback(std::function<void(Connection*)> fn)
{
    closecallback_ = fn;
}
// 设置fd_发生了错误的回调函数。
void Connection::seterrorcallback(std::function<void(Connection*)> fn)
{
    errorcallback_ = fn;
}
