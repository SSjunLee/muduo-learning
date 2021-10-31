#pragma once
#include<vector>
#include<map>
#include"Timestamp.h"
class EventLoop;
class Channel;
class Epoller
{

private:
    static const int defaultEventsSize = 16;

public:
    typedef std::vector<struct epoll_event> EventList;
    typedef std::vector<Channel*> ChannelList;
    typedef std::map<int,Channel*> ChannelMap;
private:

    EventLoop* ownerLoop_;
    int epfd_;
    EventList events_;
    //fd 到 Channel 的映射
    ChannelMap channels_;

public:

    Timestamp poll(ChannelList*activeChannels,int timeOutMs);
    Epoller(EventLoop* loop);
    ~Epoller();
    //修改io事件，必须被loop线程调用
    void updateChannel(Channel*channel);
private:
    void fillActiveChannels(int numEvents,ChannelList*activeChannels) const;    
    inline void assertInLoopThread(){ownerLoop_->assertInLoopThread();}
};

