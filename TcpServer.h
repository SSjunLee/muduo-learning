#pragma once
#include "InetAddress.h"
#include "Type.h"
#include <map>
#include <memory>
class EventLoop;
class Acceptor;
class TcpServer:nocopyable
{
public:
    TcpServer(EventLoop*loop,const InetAddress& listenAddr);
    ~TcpServer();
    void start();   
    void setConnectionCallback(const ConnectionCallback&cb){connectionCallback_ =cb;};
    void setMessageCallback(const MessageCallback&cb){meassageCallback = cb;};
    void setWriteCompleteCallback(const WriteCompleteCallback&cb){writeCompleteCallback_ = cb;}

private:
    void newConnection(int sockfd,const InetAddress& peerAddr);
    void removeConnection(const TcpConnectionPtr&con);

private:
    typedef std::map<std::string,TcpConnectionPtr> ConnectionMap;
    EventLoop* loop_;
    const std::string name_;
    std::unique_ptr<Acceptor> acceptor_;
    ConnectionCallback connectionCallback_;
    MessageCallback meassageCallback;
    WriteCompleteCallback writeCompleteCallback_;
    bool started_;
    int nextConnId_;
    ConnectionMap connections_;
};

