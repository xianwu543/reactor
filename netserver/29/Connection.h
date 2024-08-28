#pragma once
#include <functional>
#include "Socket.h"
#include "InetAddress.h"
#include "Channel.h"
#include "EventLoop.h"
#include"Buffer.h"
#include<memory>
#include<atomic>
class EventLoop;
class Channel;

class Connection;
using spConnection = std::shared_ptr<Connection>;

class Connection:public std::enable_shared_from_this<Connection>
{
private:
    EventLoop *loop_;
    Socket *clientsock_;
    Channel *clientchannel_;
    std::function<void(spConnection)> closecallback_;                   // 关闭fd_的回调函数，将回调TcpServer::closeconnection()。
    std::function<void(spConnection)> errorcallback_;                   // fd_发生了错误的回调函数，将回调TcpServer::errorconnection()。
    std::function<void(spConnection,std::string&)> onmessagecallback_;   // 处理报文的回调函数，将回调TcpServer::onmessage()。
    std::function<void(spConnection)> sendcompletecallback_;               // 发送数据完成后的回调函数，将回调TcpServer::sendcomplete()。

    Buffer inputbuffer_;
    Buffer outputbuffer_;

    std::atomic_bool  disconnect_;  //客户端连接是否已断开，如果已断开设置为true。

public:
    Connection(EventLoop *loop,Socket *clientsock);
    ~Connection();

    int fd() const;
    std::string ip() const;
    uint16_t port() const;

    void onmessage();
    void closecallback();                    // TCP连接关闭（断开）的回调函数，供Channel回调。
    void errorcallback();                    // TCP连接错误的回调函数，供Channel回调。
    void writecallback();                   // 处理写事件的回调函数，供Channel回调。

    void setclosecallback(std::function<void(spConnection)> fn);    // 设置关闭fd_的回调函数。
    void seterrorcallback(std::function<void(spConnection)> fn);    // 设置fd_发生了错误的回调函数。
    void setonmessagecallback(std::function<void(spConnection,std::string&)> fn);    // 设置处理报文的回调函数。
    void setsendcompletecallback(std::function<void(spConnection)> fn);    // 发送数据完成后的回调函数。

    void send(const char *data,size_t size);        // 发送数据。
};