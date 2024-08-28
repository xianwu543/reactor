#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <strings.h>
#include <string.h>
#include <sys/epoll.h>
#include <vector>
#include <unistd.h>

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

    void epoll_add(int epollfd,int opt);
    std::vector<struct epoll_event> loop(int timeout=-1);
};