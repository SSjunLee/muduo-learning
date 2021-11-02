#pragma once
#include"Socket.h"
#include"Channel.h"
class EventLoop;
class InetAddress;
class Acceptor
{
public:
    typedef std::function<void(int sockfd,const InetAddress&)> NewConnectionCallback;
    Acceptor(EventLoop*loop,const InetAddress& listenAddr);
    void listen();
private:
    EventLoop*loop_; 
    Socket acceptSocket_;
    Channel acceptChannel_;
    NewConnectionCallback newConnectionCallback_;
    bool listenning_;
};

