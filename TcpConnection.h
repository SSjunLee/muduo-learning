#pragma once
#include "Type.h"
#include "InetAddress.h"
#include "Buffer.h"
class EventLoop;
class Socket;
class Channel;
class TcpConnection : nocopyable, public std::enable_shared_from_this<TcpConnection>
{

public:
    TcpConnection(EventLoop *loop, const std::string &name, int sockfd,
                  const InetAddress &localAddr, const InetAddress &peerAddr);
    ~TcpConnection();
    void setConnectionCallback(const ConnectionCallback &cb) { connectionCallback_ = cb; }
    void setMessageCallback(const MessageCallback &cb) { messageCallback_ = cb; }
    void setCloseCallback(const CloseCallback &cb) { closeCallback_ = cb; }

    void connectionEstablished();
    void connectionDestroyed();
    bool connected() const { return state_ == kConnected; }
    const InetAddress &peerAddress() const { return peerAddr_; }
    const std::string &name() const { return name_; }
    void send(const void*msg,size_t len);
    void send(const std::string&msg);
    void shutdown();
private:
    void sendInLoop(const std::string&msg);
    void shutdownInLoop();
    void handleRead(Timestamp recieveTime);
    void handleWrite();
    void handleClose();
    void handleError();

private:
    enum StateE
    {
        kConnecting,
        kConnected,
        kDisConnected,
        kDisConnecting
    };
    EventLoop *loop_;
    std::string name_;
    StateE state_;
    std::unique_ptr<Socket> socket_;
    std::unique_ptr<Channel> channel_;
    InetAddress localAddr_;
    InetAddress peerAddr_;
    Buffer inputBuffer_;
    Buffer outputBuffer_;

    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;
    CloseCallback closeCallback_;
};
