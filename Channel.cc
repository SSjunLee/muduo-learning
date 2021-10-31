#include<poll.h>
#include "Channel.h"
#include"EventLoop.h"
const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = POLLIN|POLLPRI;
const int Channel::kWriteEvent = POLLOUT;


void Channel::update(){
    loop_->updateChannel(this);
}


Channel::Channel(EventLoop* loop, int fd)
:loop_(loop),
fd_(fd),
events_(0),
revents_(0),
index_(-1)
{
}

Channel::~Channel()
{
}