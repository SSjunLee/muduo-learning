#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
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

void sockets::fromHostPort(const char *ip, uint16_t port, struct sockaddr_in *addr)
{
    addr->sin_family = AF_INET;
    addr->sin_port = hostToNetwork16(port);
    if (::inet_pton(AF_INET, ip, &addr->sin_addr) <= 0)
    {
        LOG_WARN << "sockets::fromHostPort";
    }
}

void sockets::toHostPort(char *buf, size_t size,const struct sockaddr_in *addr)
{
    char host[INET_ADDRSTRLEN] = "INVALID";
    ::inet_ntop(AF_INET, &addr->sin_addr, host, sizeof host);
    uint16_t port = hostToNetwork16(addr->sin_port);
    snprintf(buf, size, "%s:%u", host, port);
}