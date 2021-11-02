#include "EventLoopThread.h"
#include "EventLoop.h"
#include "Type.h"
EventLoopThread::EventLoopThread()
    : cond_(),
      loop_(NULL),
      mutex_(),
      exiting_(false),
      thread_()
{
}

EventLoopThread::~EventLoopThread()
{
    exiting_ = true;
    loop_->quit();
    thread_.join();
}

EventLoop *EventLoopThread::startLoop()
{
    thread_ = std::thread((std::bind(&EventLoopThread::threadFunc, this)));
    {
        UniqueLockGuard g(mutex_);
        cond_.wait(g,[this](){return this->loop_!=nullptr;});
    }
    return loop_;
};

void EventLoopThread::threadFunc(){
    EventLoop loop;
    {
        UniqueLockGuard guard(mutex_);
        loop_ = &loop;
        cond_.notify_all();
    }
    loop_->loop();
}