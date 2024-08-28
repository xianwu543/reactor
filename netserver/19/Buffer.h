#pragma once
#include <string>
#include <iostream>

class Buffer
{
private:
    std::string buf_;
public:
    Buffer();
    ~Buffer();

    void append(const char *data,size_t size);
    int size();
    void clear();
    const char *data();
};