#include <poll.h>
#include <sstream>
#include <assert.h>
#include "Channel.h"
#include "EventLoop.h"
const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = POLLIN | POLLPRI;
const int Channel::kWriteEvent = POLLOUT;

void Channel::update()
{
    loop_->updateChannel(this);
}

void Channel::handleEvent(Timestamp recieveTime)
{
    eventHandling_ = true;
    if(revents_&POLLNVAL){
        LOG_WARN<<"Channel::handleEvent() POLLNAV"<<ENDL;
    }
    if((revents_& POLLHUP) && !(revents_ & POLLIN))
    {
        LOG_WARN<<"Channel::handleEvent() POLLHUP"<<ENDL;
        if(closeCallback_)closeCallback_();
    }

    if(revents_&(POLLERR|POLLNVAL))
    {
        if(errorCallback_)errorCallback_();
    }
    if(revents_&(POLLIN|POLLPRI|POLLRDHUP))
    {
        if(readCallback_)readCallback_(recieveTime);
    }
    if(revents_&(POLLOUT))
    {
        if(writeCallback_)writeCallback_();
    }
    eventHandling_ = false;
}


Channel::Channel(EventLoop *loop, int fd)
    : loop_(loop),
      fd_(fd),
      events_(0),
      revents_(0),
      index_(-1),
      eventHandling_(false)
{
};

Channel::~Channel(){
    assert(eventHandling_ == false);
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

