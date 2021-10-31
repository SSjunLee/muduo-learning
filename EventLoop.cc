#include <assert.h>
#include "EventLoop.h"


__thread EventLoop* t_loopInThisThread = 0;
EventLoop::EventLoop()
:threadId_(std::this_thread::get_id()),
looping_(false)
{
    LOG<<"EventLoop "<<this<<" created in thread "<< threadId_<<ENDL;
    if(t_loopInThisThread){
       LOG<<"Another EventLoop "<<t_loopInThisThread<<"extis in this thread "<<threadId_
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

    LOG<<"EventLoop "<<this<<" stop looping "<<ENDL;
    looping_ = false;
}

void EventLoop::updateChannel(Channel* c){

}