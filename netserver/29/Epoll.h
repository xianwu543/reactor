#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <strings.h>
#include <string.h>
#include <sys/epoll.h>
#include <vector>
#include <unistd.h>
#include"Channel.h"

// 前向声明 Channel 类
class Channel;

// Epoll类。
class Epoll
{
private:
    int epollfd_ = -1;
    static const int MAXSIZE=100;
    struct epoll_event events_[MAXSIZE];
public:
    Epoll();
    ~Epoll();

    // void epoll_add(int fd,int opt);
    void updatechannel(Channel *ch);
    void removechannel(Channel *ch);

    std::vector<Channel*> loop(int timeout=-1);
};
