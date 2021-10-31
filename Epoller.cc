#include<sys/epoll.h>
#include<assert.h>
#include"Epoller.h"
#include"EventLoop.h"
#include"Channel.h"

Epoller::Epoller(EventLoop*loop)
:ownerLoop_(loop),
epfd_(epoll_create1(EPOLL_CLOEXEC)),
events_(defaultEventsSize),
channels_()
{
    if(epfd_<0){
        LOG<<"epfd 错误"<<ENDL;
    }
}

Epoller::~Epoller()
{
}


Timestamp Epoller::poll(ChannelList*activeChannels,int timeOutMs){
    int numEvents =epoll_wait(epfd_
                ,&*events_.begin()
                ,static_cast<int>(events_.size()),
                timeOutMs);
    int save_errorno = errno;       
    Timestamp time =  Timestamp::now();     
    if(numEvents > 0)
    {
        LOG<<numEvents<<" events happened..."<<ENDL;
        fillActiveChannels(numEvents,activeChannels);
        if(static_cast<size_t>(numEvents) == events_.size())
            events_.resize(2*events_.size());

    }else  if(numEvents < 0)
    {
        if(save_errorno!=EINTR)
        {
            errno = save_errorno;
            LOG<<"Epoller::poller"<<ENDL;
        }
    }            
    return time;
}


void Epoller::fillActiveChannels(int numEvents,ChannelList*activeChannels) const
{
    assert(static_cast<size_t>(numEvents) <= events_.size());
    for(int i =0;i<numEvents;i++)
    {
        Channel* channel = static_cast<Channel*>(events_[i].data.ptr);
        #ifndef NODEBUG
        int fd = channel->fd();
        auto it = channels_.find(fd);
        assert(it!=channels_.end());
        assert(it->second== channel);
        #endif

        channel->set_revents(events_[i].events);
        activeChannels->push_back(channel);


    }
}