#include <sys/epoll.h>
#include <cstring>
#include <assert.h>
#include "Epoller.h"
#include "EventLoop.h"
#include "Channel.h"

Epoller::Epoller(EventLoop *loop)
    : ownerLoop_(loop),
      epfd_(epoll_create1(EPOLL_CLOEXEC)),
      events_(defaultEventsSize),
      channels_()
{
    if (epfd_ < 0)
    {
        LOG_SYSFATAL << "epfd 错误 " << ENDL;
        abort();
    }
}

Epoller::~Epoller()
{
}

Timestamp Epoller::poll(ChannelList *activeChannels, int timeOutMs)
{
    int numEvents = epoll_wait(epfd_, &*events_.begin(), static_cast<int>(events_.size()),
                               timeOutMs);
    int save_errorno = errno;
    Timestamp time = Timestamp::now();
    if (numEvents > 0)
    {
        //LOG << numEvents << " events happened..." << ENDL;
        fillActiveChannels(numEvents, activeChannels);
        if (static_cast<size_t>(numEvents) == events_.size())
            events_.resize(2 * events_.size());
    }
    else if (numEvents < 0)
    {
        if (save_errorno != EINTR)
        {
            errno = save_errorno;
            LOG << "Epoller::poller" << ENDL;
        }
    }
    return time;
}

void Epoller::fillActiveChannels(int numEvents, ChannelList *activeChannels) const
{
    assert(static_cast<size_t>(numEvents) <= events_.size());
    for (int i = 0; i < numEvents; i++)
    {
        Channel *channel = static_cast<Channel *>(events_[i].data.ptr);
#ifndef NODEBUG
        int fd = channel->fd();
        auto it = channels_.find(fd);
        assert(it != channels_.end());
        assert(it->second == channel);
#endif

        channel->set_revents(events_[i].events);
        activeChannels->push_back(channel);
    }
}

const char *Epoller::opToString(int op)
{
    switch (op)
    {
    case EPOLL_CTL_ADD:
        return "ADD";
    case EPOLL_CTL_MOD:
        return "MOD";
    case EPOLL_CTL_DEL:
        return "DEL";
    default:
        return "UNKOWN";
    }
}

void Epoller::update(int op, Channel *channel)
{
    struct epoll_event event;
    memset(&event, 0, sizeof event);
    event.data.ptr = channel;
    event.events = channel->events();
    LOG << "epoll_ctl op =" << opToString(op) << "event = {" << channel->eventsToString(channel->fd(), channel->events()) << " }" << ENDL;
    if (::epoll_ctl(epfd_, op, channel->fd(), &event) < 0)
    {   
        if(op != EPOLL_CTL_DEL)
        LOG_SYSFATAL << " epoll ctl op = " << opToString(op) << " "<<ENDL;
    }
}

void Epoller::updateChannel(Channel *channel)
{
    assertInLoopThread();
    const int index = channel->index();
    LOG_DEBUG<<"fd = "<<channel->fd()<<" events = "<<
    channel->eventsToString(channel->fd(),channel->events())<<" type =  "<<
    index<<ENDL;
    if (index == knew || index == kdeleted)
    {
        int fd = channel->fd();
        if (index == knew)
        {
            assert(channels_.find(fd) == channels_.end());
            channels_[fd] = channel;
        }
        else
        {
            assert(channels_.find(fd) != channels_.end());
            assert(channels_[fd] == channel);
        }
        channel->set_index(kadded);
        update(EPOLL_CTL_ADD, channel);
    }
    else
    {
        int fd = channel->fd();
        assert(channels_.find(fd) != channels_.end());
        assert(channels_[fd] == channel);
        if (channel->isNoneEvent())
        {
            update(EPOLL_CTL_DEL, channel);
            channel->set_index(kdeleted);
        }
        else
            update(EPOLL_CTL_MOD, channel);
    }
}

void Epoller::removeChannel(Channel *channel)
{
    assertInLoopThread();
    int fd = channel->fd();
    LOG<<"fd = "<<fd<<ENDL;
    assert(channels_.find(fd)!=channels_.end());
    assert(channel->isNoneEvent());
    int idx = channel->index();
    assert(idx == kadded || idx == kdeleted);
    size_t n = channels_.erase(fd);
    assert(n == 1);
    if(idx == kadded)
    {
        update(EPOLL_CTL_DEL,channel);
    }
    channel->set_index(knew);
}


