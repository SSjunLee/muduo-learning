#include "EventLoopThreadPool.h"
#include "EventLoop.h"
#include <assert.h>
EventLoopThreadPool::EventLoopThreadPool(EventLoop *baseLoop)
    : baseLoop_(baseLoop),
      numThreads_(0),
      started_(false),
      next_(0)
{
}

EventLoopThreadPool::~EventLoopThreadPool()
{
}

void EventLoopThreadPool::start()
{
    assert(started_ == false);
    baseLoop_->assertInLoopThread();
    started_ = true;
    for (int i = 0; i < numThreads_; i++)
    {
        std::shared_ptr<EventLoopThread> t(new EventLoopThread());
        threads_.push_back(t);
        loops_.push_back(t->startLoop());
    }
}

EventLoop* EventLoopThreadPool::getNextLoop()
{
    baseLoop_->assertInLoopThread();
    EventLoop *loop = baseLoop_;
    if(!loops_.empty()){
        loop = loops_[next_];
        ++next_;
        if(static_cast<size_t>(next_) == loops_.size())
            next_ = 0;
    }
    return loop;
}

