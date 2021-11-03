#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include<cstring>
#include <unistd.h>
#include "SocketOps.h"
#include "Log.h"

int sockets::createNoBlockingOrDie()
{
    int sockfd = ::socket(AF_INET,
                          SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC,
                          IPPROTO_TCP);
    if (sockfd < 0)
    {
        LOG_SYSFATAL << "sockets::createNonblockingOrDie" << ENDL;
    }
    return sockfd;
}

void sockets::bindOrDie(int sockfd, const struct sockaddr_in &sockAddr)
{
    int ret = ::bind(sockfd, reinterpret_cast<const struct sockaddr *>(&sockAddr), sizeof(sockAddr));
    if (ret < 0)
    {
        LOG_SYSFATAL << "sockets::bindOrDie";
    }
}

void sockets::listenOrDie(int sockfd)
{
    int ret = ::listen(sockfd, SOMAXCONN);
    if (ret < 0)
    {
        LOG_SYSFATAL << "sockets::listenOrDie" << ENDL;
    }
}

int sockets::accept(int sockfd, struct sockaddr_in *addr)
{
    socklen_t len = sizeof(*addr);
    int connfd = ::accept4(sockfd, reinterpret_cast<struct sockaddr *>(addr), &len, SOCK_CLOEXEC | SOCK_NONBLOCK);
    if (connfd < 0)
    {
        int savedErrno = errno;
        LOG_WARN << "sockets::accept() " << ENDL;
        switch (savedErrno)
        {
        case EAGAIN:
        case ECONNABORTED:
        case EINTR:
        case EPROTO: // ???
        case EPERM:
        case EMFILE: // per-process lmit of open file desctiptor ???
            // expected errors
            errno = savedErrno;
            break;
        case EBADF:
        case EFAULT:
        case EINVAL:
        case ENFILE:
        case ENOBUFS:
        case ENOMEM:
        case ENOTSOCK:
        case EOPNOTSUPP:
            // unexpected errors
            LOG_SYSFATAL << "unexpected error of ::accept " << savedErrno;
            break;
        default:
            LOG_SYSFATAL << "unknown error of ::accept " << savedErrno;
            break;
        }
    }
    return connfd;
}

void sockets::close(int fd)
{
    int ret = ::close(fd);
    if (ret < 0)
    {
        LOG_SYSFATAL << "sockets::close" << ENDL;
    }
}

void sockets::fromHostPort(const char *ip, uint16_t port, struct sockaddr_in *addr)
{
    addr->sin_family = AF_INET;
    addr->sin_port = hostToNetwork16(port);
    if (::inet_pton(AF_INET, ip, &addr->sin_addr) <= 0)
    {
        LOG_WARN << "sockets::fromHostPort";
    }
}

void sockets::toHostPort(char *buf, size_t size, const struct sockaddr_in *addr)
{
    char host[INET_ADDRSTRLEN] = "INVALID";
    ::inet_ntop(AF_INET, &addr->sin_addr, host, sizeof host);
    uint16_t port = hostToNetwork16(addr->sin_port);
    snprintf(buf, size, "%s:%u", host, port);
}


struct sockaddr_in sockets::getLocalAddr(int sockfd)
{
    struct sockaddr_in res;
    bzero(&res,sizeof(res));
    socklen_t len = sizeof(res);
    int ret = ::getsockname(sockfd,reinterpret_cast<struct sockaddr*>(&res),&len);
    if(ret < 0)
    {
        LOG_SYSFATAL<<"sockets::getLocalAddr "<<ENDL;
    }
    return res;
}

int sockets::getSocketError(int fd){
    int optval;
    socklen_t optlen = sizeof(optval);
    if(::getsockopt(fd,SOL_SOCKET,SO_ERROR,&optval,&optlen) < 0)
    {
        return errno;
    }else 
    {
        return optval;
    }

}