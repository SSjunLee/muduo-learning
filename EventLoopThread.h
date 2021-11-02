#pragma once
#include"Type.h"
#include<thread>
#include<condition_variable>
#include<mutex>
class EventLoop;
class EventLoopThread:nocopyable
{
public:
    EventLoopThread();
    ~EventLoopThread();
    EventLoop* startLoop();
private:
    void threadFunc();        
private:
    bool exiting_;
    EventLoop* loop_;
    std::thread thread_;
    std::mutex mutex_;
    std::condition_variable cond_;
};


