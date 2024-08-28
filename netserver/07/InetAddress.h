#pragma once
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>

class InetAddress
{
private:
    struct sockaddr_in sddr_;
public:
    InetAddress(const std::string &ip,uint16_t port);
    InetAddress(struct sockaddr_in sddr);
    InetAddress();
    ~InetAddress();

    char *ip() const;
    uint16_t port() const;
    struct sockaddr* addr() const;
    void setaddr(sockaddr_in clientaddr);   // 设置addr_成员的值。

};