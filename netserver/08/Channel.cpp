#include"Channel.h"

Channel::Channel(Epoll* ep,int fd):ep_(ep),fd_(fd)
{

}
Channel::~Channel()
{
    //ep_和fd_都是借用，不参与他们的释放权力。
}

int Channel::fd()
{
    return fd_;
}
void Channel::enablereading()
{
    events_ = events_|EPOLLIN;
    ep_->updatechannel(this);
}
void Channel::setinepoll()
{
    inepoll_ = true;
}
bool Channel::inepoll()
{
    return inepoll_;
}
void Channel::setrevents(uint32_t op)
{
    revents_ = op;
}
uint32_t Channel::events()
{
    return events_;
}
uint32_t Channel::revents()
{
    return revents_;
}

void Channel::useet()
{
    events_ = events_|EPOLLET;
}

void Channel::handleevent()
{
    ////////////////////////////////////////////////////////////////////////
    if (revents_ & EPOLLRDHUP)                     // 对方已关闭，有些系统检测不到，可以使用EPOLLIN，recv()返回0。
    {
        printf("client(eventfd=%d) disconnected.\n",fd_);
        close(fd_);            // 关闭客户端的fd。
    }                                //  普通数据  带外数据
    else if (revents_ & (EPOLLIN|EPOLLPRI))   // 接收缓冲区中有数据可以读。
    {
        readcallback_();    // 每个channel(监听的channel和客户端channel分别执行初始化时设置好的自身的函数)。
    }
    else if (revents_ & EPOLLOUT)                  // 有数据需要写，暂时没有代码，以后再说。
    {
    }
    else                                                                    // 其它事件，都视为错误。
    {
        printf("client(eventfd=%d) error.\n",fd_);
        close(fd_);            // 关闭客户端的fd。
    }
    ////////////////////////////////////////////////////////////////////////

}

// 处理新客户端连接请求。
void Channel::newconnection(Socket *servsock)
{
    ////////////////////////////////////////////////////////////////////////
    InetAddress clientaddr;
    Socket* clientsock = new Socket(servsock->accept(clientaddr));

    printf ("accept client(fd=%d,ip=%s,port=%d) ok.\n",clientsock->fd(),clientaddr.ip(),clientaddr.port());

    // 为新客户端连接准备读事件，并添加到epoll中。
    Channel *clientchannel = new Channel(ep_,clientsock->fd());
    clientchannel->setreadcallback(std::bind(&Channel::onmessage,clientchannel));
    clientchannel->useet();
    clientchannel->enablereading();
    ////////////////////////////////////////////////////////////////////////
}

// 处理对端发送过来的消息。
void Channel::onmessage()
{
    char buffer[1024];
    while (true)             // 由于使用非阻塞IO，一次读取buffer大小数据，直到全部的数据读取完毕。
    {    
        bzero(&buffer, sizeof(buffer));
        ssize_t nread = read(fd_, buffer, sizeof(buffer));    // 这行代码用了read()，也可以用recv()，一样的，不要纠结。
        if (nread > 0)      // 成功的读取到了数据。
        {
            // 把接收到的报文内容原封不动的发回去。
            printf("recv(eventfd=%d):%s\n",fd_,buffer);
            send(fd_,buffer,strlen(buffer),0);
        } 
        else if (nread == -1 && errno == EINTR) // 读取数据的时候被信号中断，继续读取。
        {  
            continue;
        } 
        else if (nread == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))) // 全部的数据已读取完毕。
        {
            break;
        } 
        else if (nread == 0)  // 客户端连接已断开。
        {  
            printf("client(eventfd=%d) disconnected.\n",fd_);
            close(fd_);            // 关闭客户端的fd。
            break;
        }
    }
}                             



void Channel::setreadcallback(std::function<void()> fn)
{
    readcallback_ = fn;
}