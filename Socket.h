#pragma once
#include "Type.h"
class InetAddress;
class Socket:nocopyable
{

public:
    explicit Socket(int sockFd):sockFd_(sockFd)
    {}
    ~Socket();
    int fd() const {return sockFd_;}
    void bindAddress(const InetAddress& localAddress);
    void listen();
    void accept(InetAddress* address);
    void setReusedAddr(bool on);

private:
    int sockFd_;
  
};

