#include"Acceptor.h"
#include "EventLoop.h"
#include "InetAddress.h"
#include "SocketOps.h"
Acceptor::Acceptor(EventLoop*loop,const InetAddress& listenAddr):
loop_(loop),
acceptSocket_(sockets::createNoBlockingOrDie()),
acceptChannel_(loop,acceptSocket_.fd()),
listenning_(false)
{
    acceptSocket_.setReusedAddr(true);
    acceptSocket_.bindAddress(listenAddr);
    acceptChannel_.setReadCallback_(std::bind(&Acceptor::handleRead,this));  
}

void Acceptor::listen()
{
    loop_->assertInLoopThread();
    acceptSocket_.listen();
    acceptChannel_.enableReading();
}

void Acceptor::handleRead(){
    loop_->assertInLoopThread();
    InetAddress clientAddr(0);
    int confd = acceptSocket_.accept(&clientAddr);
    if(confd>=0){
        if(newConnectionCallback_)newConnectionCallback_(confd,clientAddr);
    }else sockets::close(confd);
}