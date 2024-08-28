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
    conn->setclosecallback(std::bind(&TcpServer::closeconnection,this,std::placeholders::_1));
    conn->seterrorcallback(std::bind(&TcpServer::errorconnection,this,std::placeholders::_1));
    conn->setonmessagecallback(std::bind(&TcpServer::onmessage,this,std::placeholders::_1,std::placeholders::_2));
    printf ("accept client(fd=%d,ip=%s,port=%d) ok.\n",conn->fd(),conn->ip().c_str(),conn->port());

    conns_[conn->fd()] = conn;    //把conn存放在map容器中。
}

// 关闭客户端的连接，在Connection类中回调此函数。
void TcpServer::closeconnection(Connection *conn)
{
    printf("client(eventfd=%d) disconnected.\n",conn->fd());
    // close(conn->fd());            // 关闭客户端的fd。
    conns_.erase(conn->fd());
    delete conn;
}
// 客户端的连接错误，在Connection类中回调此函数。
void TcpServer::errorconnection(Connection *conn)
{
    printf("client(eventfd=%d) error.\n",conn->fd());
    // close(conn->fd());            // 关闭客户端的fd。
    conns_.erase(conn->fd());
    delete conn;
}

//处理客户端的请求报文，再Connection类中回调此函数。
void TcpServer::onmessage(Connection *conn,std::string message)
{
    // 若干计算
    message = "reply:"+message;

    //发送
    int len = message.size();
    char tmp[1028];
    memset(tmp,0,sizeof(tmp));
    memcpy(tmp,&len,4);
    memcpy(tmp+4,message.data(),len);
    // send(conn->fd(),tmp,len+4,0);
    conn->send(tmp,len+4);
}