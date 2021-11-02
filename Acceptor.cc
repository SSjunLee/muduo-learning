#include"Acceptor.h"
#include "SocketOps.h"
Acceptor::Acceptor(EventLoop*loop,const InetAddress& listenAddr):
loop_(loop),
acceptSocket_(sockets::createNoBlockingOrDie()),
acceptChannel_(loop,acceptSocket_.fd()),
listenning_(false)
{
}