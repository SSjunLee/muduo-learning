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
      peerAddr_(peerAddr_),
      inputBuffer_(),
      outputBuffer_()
{
  LOG_DEBUG << "TcpConnetion create [" << name_ << "] at fd = " << sockfd << ENDL;
  channel_->setReadCallback_(std::bind(&TcpConnection::handleRead, this, std::placeholders::_1));
  channel_->setWriteCallback(std::bind(&TcpConnection::handleWrite, this));
  channel_->setErrorCallback(std::bind(&TcpConnection::handleError, this));
  channel_->setCloseCallback(std::bind(&TcpConnection::handleClose, this));

  //channel_->enableReading();
}

//如果对端关闭，则必须关闭连接，否则会一直read 0byte,poller会不停地触发读事件，导致忙轮询
void TcpConnection::handleRead(Timestamp recieveTime)
{
  int saveError = 0;
  ssize_t n = inputBuffer_.readFd(channel_->fd(), &saveError);
  if (n > 0)
    messageCallback_(shared_from_this(), &inputBuffer_, recieveTime);
  else if (n == 0)
  {
    handleClose();
  }
  else
  {
    errno = saveError;
    LOG_ERROR << "TcpConnection::handleRead";
    handleError();
  }
}
void TcpConnection::handleWrite()
{
  loop_->assertInLoopThread();
  //将输出缓冲区里的数据写出
  if (channel_->isWriting())
  {
    ssize_t n = ::write(channel_->fd(), outputBuffer_.peek(), outputBuffer_.readableBytes());
    if (n > 0)
    {
      outputBuffer_.retrieve(n);
      if (outputBuffer_.readableBytes() == 0)
      {
        channel_->disableWriting();
        if(writeCompleteCallback_)
          loop_->queueInLoop(std::bind(writeCompleteCallback_,shared_from_this()));
        if (state_ == kDisConnecting)
        {
          shutdownInLoop();
        }
        else
        {
          LOG << "write more data" << ENDL;
        }
      }
    }
    else
    {
      LOG_ERROR << "TcpConnection::handleWrite()";
    }
  }
  else
  {
    LOG << "connect is down,no more writing" << ENDL;
  }
}
void TcpConnection::handleClose()
{
  loop_->assertInLoopThread();
  LOG << "TcpConnection::handleClose() state = " << state_ << ENDL;
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
  loop_->removeChannel(channel_.get());
}

void TcpConnection::send(const void *msg, size_t len)
{
}
void TcpConnection::send(const std::string &msg)
{ 
  //channel_->enableWriting();
  //return;
  if (state_ == kConnected)
  {
    if (loop_->isInLoopThread())
    {
      sendInLoop(msg);
    }
    else
    {
      loop_->runInLoop(std::bind(&TcpConnection::sendInLoop, shared_from_this(), msg));
    }
  }
}

void TcpConnection::sendInLoop(const std::string &msg)
{
  loop_->assertInLoopThread();
  ssize_t nwrote = 0;
  //先尝试直接把msg的数据发送
  if (!channel_->isWriting() && outputBuffer_.readableBytes() == 0)
  {
    nwrote = ::write(channel_->fd(), msg.data(), msg.size());
    if (nwrote >= 0)
    {
      if (static_cast<size_t>(nwrote) < msg.size())
        LOG << "I m going to write more data" << ENDL;
    }
    else
    {
      nwrote = 0;
      if (errno == EWOULDBLOCK)
        LOG_ERROR << "TcpConnection::sendInLoop" << ENDL;
    }
  }
  assert(nwrote >= 0);
  //如果没写完，则把没写完的append到缓冲区里，让outbuffer写
  if (static_cast<size_t>(nwrote) < msg.size())
  {
    outputBuffer_.append(msg.data() + nwrote, msg.size() - nwrote);
    if (!channel_->isWriting())
    {
      channel_->enableWriting();
    }
  }
}

void TcpConnection::shutdown()
{
  if (state_ == kConnected)
  {
    state_ = kDisConnecting;
    loop_->runInLoop(std::bind(&TcpConnection::shutdownInLoop, shared_from_this()));
  }
}

void TcpConnection::shutdownInLoop()
{
  loop_->assertInLoopThread();
  if (!channel_->isWriting())
  {
    socket_->shutdownWrite();
  }
}