#include"TcpServer.h"

TcpServer::TcpServer(const std::string &ip,uint16_t port)
{
    acceptor_ = new Acceptor(&loop_,ip,port);
    acceptor_->setnewconnectioncb(std::bind(&TcpServer::newconnection,this,std::placeholders::_1));
}
TcpServer::~TcpServer()
{
    delete acceptor_;

    //释放全部的Connection对象。
    for(auto &aa:conns_)
    {
        delete aa.second;
    }
}

void TcpServer::start()
{
    loop_.run();
}

void TcpServer::newconnection(Socket *clientsock)
{
    Connection *conn = new Connection(&loop_,clientsock);

    printf ("accept client(fd=%d,ip=%s,port=%d) ok.\n",conn->fd(),conn->ip().c_str(),conn->port());

    conns_[conn->fd()] = conn;    //把conn存放在map容器中。
}