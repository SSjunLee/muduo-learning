#include "EventLoop.h"
#include "Acceptor.h"
#include "InetAddress.h"
#include "SocketOps.h"
#include <thread>
using namespace std;

void newConnection(int sockfd, const InetAddress &peerAddr)
{
    printf("newConnection(): accepted a new connection from %s\n",
           peerAddr.toHostPort().c_str());
    ::write(sockfd, "How are you?\n", 13);
    sockets::close(sockfd);
}

void listenPort(uint16_t port)
{
    InetAddress listenAddr(port);
    EventLoop loop;
    Acceptor acceptor(&loop, listenAddr);
    acceptor.setNewConnectionCallback(newConnection);
    acceptor.listen();
    loop.loop();
}


int main()
{   
    printf("main(): pid = %d\n", getpid());
    thread t([](){listenPort(5678);});
    listenPort(1234);
    

}