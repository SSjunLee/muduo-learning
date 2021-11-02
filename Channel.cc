#include <poll.h>
#include <sstream>
#include "Channel.h"
#include "EventLoop.h"
const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = POLLIN | POLLPRI;
const int Channel::kWriteEvent = POLLOUT;

void Channel::update()
{
    loop_->updateChannel(this);
}

void Channel::handleEvent()
{
    if(revents_&POLLNVAL){
        LOG_WARN<<"Channel::handleEvent() POLLNAV"<<ENDL;
    }
    if(revents_&(POLLERR|POLLNVAL))
    {
        if(errorCallback_)errorCallback_();
    }
    if(revents_&(POLLIN|POLLPRI|POLLRDHUP))
    {
        if(readCallback_)readCallback_();
    }
    if(revents_&(POLLOUT))
    {
        if(writeCallback_)writeCallback_();
    }
}


Channel::Channel(EventLoop *loop, int fd)
    : loop_(loop),
      fd_(fd),
      events_(0),
      revents_(0),
      index_(-1)
{
}

std::string Channel::eventsToString(int fd, int ev)
{
    std::ostringstream oss;
    oss <<"fd: "<<fd << ", events:  [";
    if (ev & POLLIN)
        oss << "IN ";
    if (ev & POLLPRI)
        oss << "PRI ";
    if (ev & POLLOUT)
        oss << "OUT ";
    if (ev & POLLHUP)
        oss << "HUP ";
    if (ev & POLLRDHUP)
        oss << "RDHUP ";
    if (ev & POLLERR)
        oss << "ERR ";
    if (ev & POLLNVAL)
        oss << "NVAL ";

    oss<<" ]";    

    return oss.str();
}

