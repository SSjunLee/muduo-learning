#include <assert.h>
#include "EventLoop.h"
#include"Epoller.h"

__thread EventLoop* t_loopInThisThread = 0;
EventLoop::EventLoop()
:threadId_(std::this_thread::get_id()),
looping_(false),
poller_(new Epoller(this)),
quit_(false),
pollReturnTime()
{
    LOG<<"EventLoop "<<this<<" created in thread "<< threadId_<<ENDL;
    if(t_loopInThisThread){
       LOG_SYSFATAL<<"Another EventLoop "<<t_loopInThisThread<<"extis in this thread "<<threadId_
       <<ENDL;
    }else
    {
        t_loopInThisThread = this;
    }
}

EventLoop::~EventLoop()
{
    assert(!looping_);
    t_loopInThisThread = NULL;
}


EventLoop* EventLoop::getEventLoopInCurrentThread(){
        return t_loopInThisThread;
}

void EventLoop::loop()
{
    assert(!looping_);
    assertInLoopThread();
    looping_ = true;
    quit_ = false;
    while(!quit_)
    {
        activeChannels_.clear();
        pollReturnTime = poller_->poll(&activeChannels_,kPollTimeMs);
        for(auto&channel:activeChannels_)
        {
            channel->handleEvent();
        }
    }
    LOG<<"EventLoop "<<this<<" stop looping "<<ENDL;
    looping_ = false;
}

void EventLoop::quit(){quit_=true;}

void EventLoop::updateChannel(Channel* c){
    assert(c->ownerLoop() ==this);
    assertInLoopThread();
    poller_->updateChannel(c);
}