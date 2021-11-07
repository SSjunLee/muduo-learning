#include "TcpServer.h"
#include "TcpConnection.h"
#include "EventLoop.h"
#include "InetAddress.h"
#include <stdio.h>

std::string message;


void onConnection(const TcpConnectionPtr &conn)
{
  if (conn->connected())
  {
    printf("onConnection(): new connection [%s] from %s\n",
           conn->name().c_str(),
           conn->peerAddress().toHostPort().c_str());
    conn->send(message);
    //conn->shutdown();
  }
  else
  {
    printf("onConnection(): connection [%s] is down\n",
           conn->name().c_str());
  }
}

void onMessage(const TcpConnectionPtr &conn,
               Buffer *buf,
               Timestamp receiveTime)
{
  printf("onMessage(): received %zd bytes from connection [%s] at %s\n",
         buf->readableBytes(),
         conn->name().c_str(),
         receiveTime.toString().c_str());

  buf->retrieveAll();
}

void onWriteComplete(const TcpConnectionPtr &conn)
{
  conn->send("onWriteComplete");
}

int main(int argc, char *argv[])
{
  printf("main(): pid = %d\n", getpid());
  int len = atoi(argv[1]);
  message.resize(len);
  std::fill(message.begin(),message.end(),'A');

  InetAddress listenAddr(1234);
  EventLoop loop;

  TcpServer server(&loop, listenAddr);
  server.setThreadNum(3);
  server.setConnectionCallback(onConnection);
  server.setMessageCallback(onMessage);
  server.setWriteCompleteCallback(onWriteComplete);
  server.start();

  loop.loop();
}
