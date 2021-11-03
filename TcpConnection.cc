#include "TcpConnection.h"
#include "EventLoop.h"
#include "Socket.h"
#include "Channel.h"
#include <assert.h>
#include "SocketOps.h"

TcpConnection::TcpConnection(EventLoop *loop, const std::string &name, int sockfd,
                             const InetAddress &localAddr, const InetAddress &peerAddr)
    : loop_(loop),
      name_(name),
      state_(kConnecting),
      socket_(new Socket(sockfd)),
      channel_(new Channel(loop_, sockfd)),
      localAddr_(localAddr_),
      peerAddr_(peerAddr_)
{
  LOG_DEBUG << "TcpConnetion create [" << name_ << "] at fd = " << sockfd << ENDL;
  channel_->setReadCallback_(std::bind(&TcpConnection::handleRead, this));
  //channel_->enableReading();
}


//如果对端关闭，则必须关闭连接，否则会一直read 0byte,poller会不停地触发读事件，导致忙轮询
void TcpConnection::handleRead()
{
  char buf[65535] = {0};
  ssize_t n = ::read(socket_->fd(), buf, sizeof(buf));
  if (n > 0)
    messageCallback_(shared_from_this(), buf, n);
  else if (n == 0)
  {
    handleClose();
  }
  else
  {
    handleError();
  }
}
void TcpConnection::handleWrite() {}
void TcpConnection::handleClose()
{
  loop_->assertInLoopThread();
  LOG << "TcpConnection::handleClose() state = " << state_;
  channel_->disableAll();
  closeCallback_(shared_from_this());
}

void TcpConnection::handleError()
{
  int err = sockets::getSocketError(channel_->fd());
  LOG_ERROR << "TcpConnection::handleError() [" << name_ << "] SO_ERROR = " 
  << debugs::geterror(err) << ENDL;
}

TcpConnection::~TcpConnection()
{
  LOG_DEBUG << "TcpConnetion destory [" << name_ << "] at fd = " << channel_->fd() << ENDL;
}

void TcpConnection::connectionEstablished()
{
  loop_->assertInLoopThread();
  //assert(state_==kConnecting);
  state_ = kConnected;
  channel_->enableReading();
  connectionCallback_(shared_from_this());
}

//某些情况下可以不经过handleClose() 直接调用这个方法
void TcpConnection::connectionDestroyed()
{
  loop_->assertInLoopThread();
  state_ = kDisConnected;
  channel_->disableAll();
  connectionCallback_(shared_from_this());
  

}