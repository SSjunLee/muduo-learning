#include <poll.h>
#include <sstream>
#include<sys/epoll.h>
#include <assert.h>
#include "Channel.h"
#include "EventLoop.h"
const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = EPOLLIN | EPOLLPRI;
const int Channel::kWriteEvent = EPOLLOUT;

void Channel::update()
{
    loop_->updateChannel(this);
}

void Channel::handleEvent(Timestamp recieveTime)
{
    eventHandling_ = true;
    if (revents_ & POLLNVAL)
    {
        LOG_WARN << "Channel::handleEvent() POLLNAV" << ENDL;
    }
    if ((revents_ & POLLHUP) && !(revents_ & POLLIN))
    {
        LOG_WARN << "Channel::handleEvent() POLLHUP" << ENDL;
        if (closeCallback_)
            closeCallback_();
    }

    if (revents_ & (POLLERR | POLLNVAL))
    {
        if (errorCallback_)
            errorCallback_();
    }
    if (revents_ & (POLLIN | POLLPRI | POLLRDHUP))
    {
        if (readCallback_)
            readCallback_(recieveTime);
    }
    if (revents_ & (POLLOUT))
    {
        if (writeCallback_)
            writeCallback_();
    }
    eventHandling_ = false;
}

Channel::Channel(EventLoop *loop, int fd)
    : loop_(loop),
      fd_(fd),
      events_(0),
      revents_(0),
      index_(-1),
      eventHandling_(false){};

Channel::~Channel()
{
    assert(eventHandling_ == false);
}

std::string Channel::eventsToString(int fd, int ev)
{
    std::ostringstream oss;
    oss << "fd: " << fd << ", events:  [";
    if (ev & POLLIN)
        oss << "POLLIN ";
    if (ev & POLLPRI)
        oss << "POLLPRI ";
    if (ev & POLLOUT)
        oss << "POLLOUT ";
    if (ev & POLLHUP)
        oss << "POLLHUP ";
    if (ev & POLLRDHUP)
        oss << "POLLRDHUP ";
    if (ev & POLLERR)
        oss << "POLLERR ";
    if (ev & POLLNVAL)
        oss << "POLLNVAL ";

    oss << " ]";

    return oss.str();
}
