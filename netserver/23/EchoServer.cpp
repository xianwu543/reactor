#include"EchoServer.h"

EchoServer::EchoServer(const std::string &ip,const uint16_t port):tcpserver_(ip,port)
{
    // 以下代码不是必须的，业务关心什么事件，就指定相应的回调函数。
    tcpserver_.setnewconnectioncb(std::bind(&EchoServer::HandleNewConnection, this, std::placeholders::_1));
    tcpserver_.setcloseconnectioncb(std::bind(&EchoServer::HandleClose, this, std::placeholders::_1));
    tcpserver_.seterrorconnectioncb(std::bind(&EchoServer::HandleError, this, std::placeholders::_1));
    tcpserver_.setonmessagecb(std::bind(&EchoServer::HandleMessage, this, std::placeholders::_1, std::placeholders::_2));
    tcpserver_.setsendcompletecb(std::bind(&EchoServer::HandleSendComplete, this, std::placeholders::_1));
    // tcpserver_.settimeoutcb(std::bind(&EchoServer::HandleTimeOut, this, std::placeholders::_1));
}

EchoServer::~EchoServer()
{

}

// 启动服务。
void EchoServer::Start()                
{
    tcpserver_.start();
}

// 处理新客户端连接请求，在TcpServer类中回调此函数。
void EchoServer::HandleNewConnection(Connection *conn)    
{
    std::cout << "New Connection Come in." << std::endl;

    // 根据业务的需求，在这里可以增加其它的代码。
}

// 关闭客户端的连接，在TcpServer类中回调此函数。 
void EchoServer::HandleClose(Connection *conn)  
{
    std::cout << "EchoServer conn closed." << std::endl;

    // 根据业务的需求，在这里可以增加其它的代码。
}

// 客户端的连接错误，在TcpServer类中回调此函数。
void EchoServer::HandleError(Connection *conn)  
{
    std::cout << "EchoServer conn error." << std::endl;

    // 根据业务的需求，在这里可以增加其它的代码。
}

// 处理客户端的请求报文，在TcpServer类中回调此函数。
void EchoServer::HandleMessage(Connection *conn,std::string message)     
{
    // 在这里，将经过若干步骤的运算。
    message = "reply:"+message;     //回显业务。

    //发送
    int len = message.size();   //计算回显报文的大小。
    char tmp[1028];
    memset(tmp,0,sizeof(tmp));
    memcpy(tmp,&len,4);         //报文头部填充到回应报文中。
    memcpy(tmp+4,message.data(),len);   //把报文内容填充到回应报文中。
    // send(conn->fd(),tmp,len+4,0);
    conn->send(tmp,len+4);          //把临时缓冲区中的数据发送出去。
}

// 数据发送完成后，在TcpServer类中回调此函数。
void EchoServer::HandleSendComplete(Connection *conn)     
{
    std::cout << "Message send complete." << std::endl;

    // 根据业务的需求，在这里可以增加其它的代码。
}

/*
// epoll_wait()超时，在TcpServer类中回调此函数。
void EchoServer::HandleTimeOut(EventLoop *loop)         
{
    std::cout << "EchoServer timeout." << std::endl;

    // 根据业务的需求，在这里可以增加其它的代码。
}
*/