#include"Connection.h"

Connection::Connection(EventLoop *loop,Socket *clientsock):loop_(loop),clientsock_(clientsock)
{
    clientchannel_ = new Channel(loop_,clientsock_->fd());
    clientchannel_->setreadcallback(std::bind(&Connection::onmessage,this));
    clientchannel_->setclosecallback(std::bind(&Connection::closecallback,this));
    clientchannel_->seterrorcallback(std::bind(&Connection::errorcallback,this));
    clientchannel_->setwritecallback(std::bind(&Connection::writecallback,this));
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

// 处理对端发送过来的消息。
void Connection::onmessage()
{
    char buffer[1024];
    while (true)             // 由于使用非阻塞IO，一次读取buffer大小数据，直到全部的数据读取完毕。
    {    
        bzero(&buffer, sizeof(buffer));
        ssize_t nread = read(fd(), buffer, sizeof(buffer));    // 这行代码用了read()，也可以用recv()，一样的，不要纠结。
        if (nread > 0)      // 成功的读取到了数据。
        {
            // 把接收到的报文内容原封不动的发回去。
            // printf("recv(eventfd=%d):%s\n",fd(),buffer);
            // send(fd(),buffer,strlen(buffer),0);
            inputbuffer_.append(buffer,nread);
        } 
        else if (nread == -1 && errno == EINTR) // 读取数据的时候被信号中断，继续读取。
        {  
            continue;
        } 
        else if (nread == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))) // 全部的数据已读取完毕。
        {
            while(true)
            {
                ///////////////////////////////////////////////////////////////////
                // 可以把以下代码封装在Buffer类中，还可以支持固定长度、指定报文长度和分隔符等多种格式。
                int len = 0;
                memcpy(&len,inputbuffer_.data(),4);
                if(inputbuffer_.size()<len+4) break;

                std::string message(inputbuffer_.data()+4,len);
                inputbuffer_.erase(0,len+4);
                ////////////////////////////////////////////////////////////////////

                printf("message (eventfd=%d):%s\n",fd(),message.c_str()); 

                // // 若干计算
                // message = "reply:"+message;

                // //发送
                // len = message.size();
                // char tmp[1028];
                // memset(tmp,0,sizeof(tmp));
                // memcpy(tmp,&len,4);
                // memcpy(tmp+4,message.data(),len);
                // send(fd(),tmp,len+4,0);
                onmessagecallback_(this,message);   // 回调TcpServer::onmessage()。
            }
            break;
        } 
        else if (nread == 0)  // 客户端连接已断开。
        {  
            closecallback();    // 回调TcpServer::closecallback()。
            break;
        }
    }
} 

// 设置处理报文的回调函数。
void Connection::setonmessagecallback(std::function<void(Connection*,std::string&)> fn)
{
    onmessagecallback_ = fn;
}

// 发送数据。
void Connection::send(const char *data,size_t size)
{
    outputbuffer_.appendwithhead(data,size);
    clientchannel_->enablewriting();
}

// 处理写事件的回调函数，供Channel回调。
void Connection::writecallback()
{
    int writen = ::send(fd(),outputbuffer_.data(),outputbuffer_.size(),0);  //尝试把outputbuffer_中的数据全部发送出去。
    if(writen>0)    outputbuffer_.erase(0,writen);  //从outputbuffer_中删除已成功发送的字节数。

    //如果发送缓冲区中没有数据了，表示数据已发送成功，不再关注写事件。
    if(outputbuffer_.size()==0)
    {
        clientchannel_->disablewriting();  
        sendcompletecallback_(this);
    }    
}

// 发送数据完成后的回调函数。
void Connection::setsendcompletecallback(std::function<void(Connection*)> fn)
{
    sendcompletecallback_ = fn;
}