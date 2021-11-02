#pragma once
#include <set>
#include <vector>
#include <functional>
#include "Channel.h"
#include "Timestamp.h"
#include "TimerId.h"
#include"Type.h"
class EventLoop;

class Timer;
class TimerQueue:nocopyable
{

public:
    typedef std::pair<Timestamp, Timer *> Entry;
    typedef std::set<Entry> TimerList;

public:
    TimerQueue(EventLoop *loop);
    ~TimerQueue();
    TimerId addTimer(const TimerCallback &cb, Timestamp when, double interval);
    std::vector<Entry> getExpired(Timestamp now);

private:
    void addTimerInLoop(Timer* timer); 
    bool insert(Timer* timer);
    void handleRead();
    void reset(std::vector<TimerQueue::Entry> expireds,Timestamp now);

private:
    EventLoop *loop_;
    const int timerfd_;
    Channel timerfdChannel_;
    TimerList timers_;
};
