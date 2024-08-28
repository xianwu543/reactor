#pragma once
#include "EventLoop.h"
#include "Socket.h"
#include "Channel.h"
#include"Acceptor.h"
#include"Connection.h"
#include<map>
#include"ThreadPool.h"

class TcpServer
{
private:
    EventLoop *mainloop_;
    std::vector<EventLoop*> subloops_;            // 存放从事件循环的容器。
    Acceptor *acceptor_;   // 一个TcpServer只有一个Acceptor对象。
    ThreadPool *threadpool_;                             // 线程池。
    int threadnum_;                                             // 线程池的大小，即从事件循环的个数。
    std::map<int,spConnection> conns_;           // 一个TcpServer有多个Connection对象，存放在map容器中。
    std::function<void(spConnection)> newconnectioncb_;          // 回调EchoServer::HandleNewConnection()。
    std::function<void(spConnection)> closeconnectioncb_;        // 回调EchoServer::HandleClose()。
    std::function<void(spConnection)> errorconnectioncb_;         // 回调EchoServer::HandleError()。
    std::function<void(spConnection,std::string &message)> onmessagecb_;        // 回调EchoServer::HandleMessage()。
    std::function<void(spConnection)> sendcompletecb_;            // 回调EchoServer::HandleSendComplete()。
    std::function<void(EventLoop*)>  timeoutcb_;                       // 回调EchoServer::HandleTimeOut()。
public:
    TcpServer(const std::string &ip,uint16_t port,int threadnum = 3);
    ~TcpServer();

    void start();
    void newconnection(Socket *clientsock);
    void closeconnection(spConnection conn);  // 关闭客户端的连接，在Connection类中回调此函数。 
    void errorconnection(spConnection conn);  // 客户端的连接错误，在Connection类中回调此函数。
    void onmessage(spConnection conn,std::string &message);   //处理客户端的请求报文，再Connection类中回调此函数。
    void sendcomplete(spConnection conn);     // 数据发送完成后，在Connection类中回调此函数。
    void epolltimeout(EventLoop *loop);         // epoll_wait()超时，在EventLoop类中回调此函数。

    void setnewconnectioncb(std::function<void(spConnection)> fn);
    void setcloseconnectioncb(std::function<void(spConnection)> fn);
    void seterrorconnectioncb(std::function<void(spConnection)> fn);
    void setonmessagecb(std::function<void(spConnection,std::string &message)> fn);
    void setsendcompletecb(std::function<void(spConnection)> fn);
    void settimeoutcb(std::function<void(EventLoop*)> fn);
};