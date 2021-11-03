#include "TcpServer.h"
#include "EventLoop.h"
#include "Acceptor.h"
#include <assert.h>
#include "SocketOps.h"
#include "TcpConnection.h"
TcpServer::TcpServer(EventLoop *loop, const InetAddress &listenAddr) : loop_(loop),
                                                                       name_(listenAddr.toHostPort()),
                                                                       acceptor_(new Acceptor(loop, listenAddr)),
                                                                       started_(false),
                                                                       nextConnId_(1)
{
    acceptor_->setNewConnectionCallback(
        std::bind(&TcpServer::newConnection, this, std::placeholders::_1, std::placeholders::_2));
}

TcpServer::~TcpServer(){}

void TcpServer::newConnection(int sockfd, const InetAddress &peerAddr)
{
    loop_->assertInLoopThread();
    char buf[32];
    snprintf(buf, sizeof buf, "#%d", nextConnId_);
    ++nextConnId_;
    std::string connName = name_ + buf;

    LOG << "TcpServer::newConnection [" << name_ << "] - new connection [" << connName << "] from " << peerAddr.toHostPort();

    InetAddress localAddr(sockets::getLocalAddr(sockfd));

    TcpConnectionPtr connectionPtr =
        std::make_shared<TcpConnection>(loop_, connName, sockfd, localAddr, peerAddr);
    connections_[connName] = connectionPtr;
    connectionPtr->setConnectionCallback(connectionCallback_);
    connectionPtr->setMessageCallback(meassageCallback);
    connectionPtr->setCloseCallback(std::bind(&TcpServer::removeConnection,this,std::placeholders::_1));
    connectionPtr->connectionEstablished();
    
}


void TcpServer::removeConnection(const TcpConnectionPtr&con)
{
    loop_->assertInLoopThread();
    LOG<<"TcpServer::removeConnection ["<<name_<<"] -connection "<<con->name()<<ENDL;
    size_t n = connections_.erase(con->name());
    assert(n == 1);
    loop_->queueInLoop(std::bind(&TcpConnection::connectionDestroyed,con));
}

void TcpServer::start()
{
    if (!started_)
    {
        started_ = true;
    }
    if (!acceptor_->listenning())
    {
        loop_->runInLoop(std::bind(&Acceptor::listen, acceptor_.get()));
    }
}