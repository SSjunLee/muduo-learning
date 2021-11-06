#pragma once
#include <vector>
#include <unistd.h>
#include <iostream>
#include <assert.h>
class Buffer
{

public:
    static const size_t kCheapPrepend = 8;
    static const size_t kInitialSize = 1024;

public:
    Buffer() : buffer_(kCheapPrepend + kInitialSize),
               readIndex_(kCheapPrepend),
               writeIndex_(kCheapPrepend){};
    ~Buffer(){};
    ssize_t readFd(int fd, int *savedErrno);
    size_t readableBytes() const {return  writeIndex_ - readIndex_; }
    size_t writeableBytes() const { buffer_.size() - writeIndex_; }
    size_t prependableBytes() const { return readIndex_; } //已经读过的垃圾数据字节数

    void makeSpace(size_t len)
    { // writeableBytes() + prependableBytes() 就是buffer_可以利用的空间
        if (writeableBytes() + prependableBytes() >= len + kCheapPrepend)
        {
            buffer_.resize(writeIndex_ + len);
        }
        else
        {
            //把还没读的数据移动到开头
            //即[readIndex ~ writeIndex] 移动到开头，把前面那些读过的垃圾数据覆盖了
            assert(kCheapPrepend < readIndex_);
            size_t readable = readableBytes();
            std::copy(begin() + readIndex_,
                      begin() + writeIndex_,
                      begin()+ kCheapPrepend);
            readIndex_ = kCheapPrepend;
            writeIndex_ = readIndex_ + readable;
            //确定指针都更新对了
            assert(readable == readableBytes());
        }
    }
    void ensureWritableBytes(size_t len)
    {
        if (writeableBytes() < len)
        {
            makeSpace(len);
        }
        assert(writeableBytes() >= len);
    }
    void append(const char *data, size_t len)
    {
        ensureWritableBytes(len);
        std::copy(data, data + len, beginWrite());
        writeIndex_+=len;
    }
    std::string retrieveAsString(){
        std::string res(peek(),readableBytes());
        retrieveAll();
        return res;
    }
    const char* peek() const {return begin() + readIndex_; }
    void retrieve(size_t n){
        readIndex_+=n;
    }
    void retrieveAll(){
        writeIndex_ = readIndex_ = kCheapPrepend;
    }

private:
    char *begin() { return &*buffer_.begin(); }
    const char *begin() const { return &*buffer_.begin(); }
    char *beginWrite() { return begin() + writeIndex_; }
    const char *beginWrite() const { return begin() + writeIndex_; }
private:
    std::vector<char> buffer_;
    size_t readIndex_;
    size_t writeIndex_;
};
