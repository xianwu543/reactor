#include"Buffer.h"

Buffer::Buffer()
{

}
Buffer::~Buffer()
{

}

void Buffer::append(const char *data,size_t size)
{
    buf_.append(data,size);
}
int Buffer::size()
{
    return buf_.size();
}
void Buffer::clear()
{
    buf_.clear();
}
const char *Buffer::data()
{
    return buf_.data();
}