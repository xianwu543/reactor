#include"Acceptor.h"

Acceptor::Acceptor(EventLoop *loop,const std::string &ip,const uint16_t port):loop_(loop)
{
    servsock_ = new Socket(createnonblocking()); //这里new出来的对象没有释放，以后再说。

    servsock_->setreuseport(1);
    servsock_->setkeepalive(1);
    servsock_->setreuseaddr(1);
    servsock_->settcpnodelay(1);

    InetAddress servaddr(ip,port);

    servsock_->bind(servaddr);
    servsock_->listen(128);

    acceptchannel_ = new Channel(loop_,servsock_->fd());
    acceptchannel_->setreadcallback(std::bind(&Acceptor::newconnection,this));
    acceptchannel_->enablereading();   
}
Acceptor::~Acceptor()
{
    delete acceptchannel_;
    delete servsock_;
}

// 处理新客户端连接请求。
void Acceptor::newconnection()
{
    ////////////////////////////////////////////////////////////////////////
    InetAddress clientaddr;
    Socket* clientsock = new Socket(servsock_->accept(clientaddr));
    //在这里setclientsock的ip和port。
    clientsock->setipport(clientaddr.ip(),clientaddr.port());

    newconnectioncb_(clientsock);
    ////////////////////////////////////////////////////////////////////////
}

void Acceptor::setnewconnectioncb(std::function<void(Socket *)> fn)
{
    newconnectioncb_ = fn;
}