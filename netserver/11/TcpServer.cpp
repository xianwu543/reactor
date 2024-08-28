#include"TcpServer.h"

TcpServer::TcpServer(const std::string &ip,uint16_t port)
{
    Socket *servsock = new Socket(createnonblocking()); //这里new出来的对象没有释放，以后再说。

    servsock->setreuseport(1);
    servsock->setkeepalive(1);
    servsock->setreuseaddr(1);
    servsock->settcpnodelay(1);

    InetAddress servaddr(ip,port);

    servsock->bind(servaddr);
    servsock->listen(128);

    Channel *servchannel = new Channel(&loop_,servsock->fd());
    servchannel->setreadcallback(std::bind(&Channel::newconnection,servchannel,servsock));
    servchannel->enablereading();    
}
TcpServer::~TcpServer()
{
}

void TcpServer::start()
{
    loop_.run();
}