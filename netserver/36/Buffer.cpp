#include "Buffer.h"

Buffer::Buffer(uint16_t sep):sep_(sep)
{

}

Buffer::~Buffer()
{

}

// 把数据追加到buf_中。
void Buffer::append(const char *data,size_t size)             
{
    buf_.append(data,size);
}

 // 把数据追加到buf_中，附加报文头部。
 void Buffer::appendwithsep(const char *data,size_t size)  
 {
    if(sep_==0)             // 没有分隔符。
    {
        buf_.append(data,size);                    // 处理报文内容。
    }
    else if(sep_==1)
    {
        buf_.append((char*)&size,4);           // 处理报文长度（头部）。
        buf_.append(data,size);                    // 处理报文内容。
    }
    // 其它的代码请各位自己完善。
 }

// 从buf_的pos开始，删除nn个字节，pos从0开始。
void Buffer::erase(size_t pos,size_t nn)                             
{
    buf_.erase(pos,nn);
}

// 返回buf_的大小。
size_t Buffer::size()                                                            
{
    return buf_.size();
}

// 返回buf_的首地址。
const char *Buffer::data()                                                  
{
    return buf_.data();
}

// 清空buf_。
void Buffer::clear()                                                            
{
    buf_.clear();
}

// 从buf_中拆分出一个报文，存放在ss中，如果buf_中没有报文，返回false。
bool Buffer::pickmessage(std::string &ss)         
{
    if (buf_.size()==0) return false;

    if (sep_==0)                  // 没有分隔符。
    {
        ss=buf_;
        buf_.clear();
    }
    else if (sep_==1)          // 四字节的报头。
    {
        //////////////////////////////////////////////////////////////
        // 可以把以下代码封装在Buffer类中，还可以支持固定长度、指定报文长度和分隔符等多种格式。
        int len;
        memcpy(&len,buf_.data(),4);     // 从inputbuffer中获取报文头部。
        // 如果inputbuffer中的数据量小于报文头部，说明inputbuffer中的报文内容不完整。
        if (buf_.size()<len+4) return false;

        ss = buf_.substr(4,len);
        buf_.erase(0,len+4);                                 // 从inputbuffer中删除刚才已获取的报文。
        //////////////////////////////////////////////////////////////
    }
    return true;
}