#include "Socket.h"
#include "SocketOps.h"
#include <cstring>
#include "InetAddress.h"
Socket::~Socket() {}
void Socket::bindAddress(const InetAddress &localAddress)
{
    sockets::bindOrDie(sockFd_, localAddress.getSocketAddrInet());
};
void Socket::listen()
{
    sockets::listenOrDie(sockFd_);
};
int Socket::accept(InetAddress *perraddr)
{
    struct sockaddr_in addr;
    bzero(&addr, sizeof(addr));
    int connfd = sockets::accept(sockFd_, &addr);
    if (connfd >= 0)
    {
        perraddr->setSocketAddrInet(addr);
    }
    return connfd;
};
void Socket::setReusedAddr(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(sockFd_, SOL_SOCKET, SO_REUSEADDR,
                 &optval, sizeof optval);
};

void Socket::shutdownWrite(){
    sockets::shutdownWrite(fd());
}