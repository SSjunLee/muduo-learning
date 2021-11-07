#pragma once
#include<vector>
#include<memory>
#include "EventLoopThread.h"

class EventLoop;
class EventLoopThreadPool
{
public:
    EventLoopThreadPool(EventLoop*baseLoop);
    ~EventLoopThreadPool();
    void setThreadNum(int numThreads){numThreads_ = numThreads;};
    void start();
    EventLoop* getNextLoop();
private:
    EventLoop* baseLoop_;
    int numThreads_;
    bool started_;
    int next_;
    std::vector<std::shared_ptr<EventLoopThread>> threads_;
    std::vector<EventLoop*> loops_;
};
