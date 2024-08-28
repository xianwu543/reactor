#include"InetAddress.h"

InetAddress::InetAddress(const std::string &ip,uint16_t port)
{
    sddr_.sin_family = AF_INET;                              // IPv4网络协议的套接字类型。
    sddr_.sin_addr.s_addr = inet_addr(ip.c_str());      // 服务端用于监听的ip地址。
    sddr_.sin_port = htons(port);               // 服务端用于监听的端口。
}
InetAddress::InetAddress(struct sockaddr_in sddr):sddr_(sddr)
{

}
InetAddress::~InetAddress()
{

}

char *InetAddress::ip() const
{
    return inet_ntoa(sddr_.sin_addr);
}
uint16_t InetAddress::port() const
{
    return (sddr_.sin_port);
}
struct sockaddr* InetAddress::addr() const
{
    return (struct sockaddr*)&sddr_;
}
