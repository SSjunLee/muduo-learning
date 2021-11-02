#include "Timer.h"
#include "Channel.h"
#include "Log.h"
#include "EventLoop.h"
#include <assert.h>
#include <sys/timerfd.h>
#include "TimeQueue.h"
#include <cstring>

namespace util
{

    int createTimerfd()
    {
        int timerfd = ::timerfd_create(CLOCK_MONOTONIC,
                                       TFD_NONBLOCK | TFD_CLOEXEC);
        if (timerfd < 0)
        {
            LOG_SYSFATAL << "Failed in timerfd_create" << ENDL;
        }
        return timerfd;
    }

    struct timespec howMuchTimeFromNow(Timestamp when)
    {
        int64_t microseonds = Timestamp::now().microSecondsSinceEpoch()-
         when.microSecondsSinceEpoch();
                              
        if (microseonds < 100)
        {
            microseonds = 100;
        }
        struct timespec res;
        res.tv_sec = static_cast<time_t>(microseonds / Timestamp::kMicroSecondPerSecond);
        res.tv_nsec = static_cast<time_t>((microseonds % Timestamp::kMicroSecondPerSecond) * 1000);
        return res;
    };

    void resetTimerFd(int timerfd, Timestamp when)
    {
        struct itimerspec oldval, newval;
        bzero(&oldval, sizeof oldval);
        bzero(&newval, sizeof newval);
        newval.it_value = howMuchTimeFromNow(when);
        int ret = timerfd_settime(timerfd, 0, &newval,&oldval);
        if (ret == -1)
            LOG_SYSFATAL << "timerfd_settime error" << ENDL;
    }

    void readTimerFd(int timerfd, Timestamp when)
    {
        uint64_t howmany = 1;
        size_t n = ::read(timerfd, &howmany, sizeof howmany);
        //LOG << "TimerQueue::handleRead() " << howmany << "  at  " << when.toString() << ENDL;
        if (n != sizeof howmany)
        {
            LOG_SYSFATAL << "TimerQueue::handleRead() reads " << n << " bytes instead of 8"
                         << ENDL;
        }
    }
};

using namespace util;

TimerQueue::TimerQueue(EventLoop *loop)
    : loop_(loop),
      timerfd_(createTimerfd()),
      timerfdChannel_(loop_, timerfd_),
      timers_()
{
    timerfdChannel_.setReadCallback_(std::bind(&TimerQueue::handleRead, this));
    timerfdChannel_.enableReading();
}

TimerQueue::~TimerQueue()
{
    ::close(timerfd_);
}

void TimerQueue::handleRead()
{
    //LOG<<"handleRead";
    loop_->assertInLoopThread();
    Timestamp now(Timestamp::now());
    readTimerFd(timerfd_, now);

    std::vector<TimerQueue::Entry> expireds = getExpired(now);
   // LOG<<expireds.size()<<std::endl;
    for (auto&entry : expireds)
    {
        entry.second->run();
        //LOG<<entry.second<<ENDL;
    }
        

    reset(expireds, now);
}

void TimerQueue::reset(std::vector<TimerQueue::Entry> expireds, Timestamp now)
{
    Timestamp nextExpired;
    for (auto &i : expireds)
    {
        if (i.second->repeat())
        {
            i.second->restart(now);
            insert(i.second);
        }
        else
            delete i.second;
    }
    if (!timers_.empty())
    {
        nextExpired = timers_.begin()->second->expiration();
    }
    if (nextExpired.valid())
    {
        resetTimerFd(timerfd_, nextExpired);
    }
}

TimerId TimerQueue::addTimer(const TimerCallback &cb, Timestamp when, double interval)
{   
   /* for(auto i:timers_){
        std::cout<<i.second->expiration().toString()<<" ";
    }
    std::cout<<ENDL;*/
    Timer *timer = new Timer(cb, interval, when);
    //timer->run();
    loop_->runInLoop(std::bind(&TimerQueue::addTimerInLoop, this, timer));
    return TimerId(timer);
}

std::vector<TimerQueue::Entry> TimerQueue::getExpired(Timestamp now)
{
    std::vector<TimerQueue::Entry> expired;
    //使查找树时不需要考虑Entry 的第二个字段
    Entry sentry = std::make_pair(now, reinterpret_cast<Timer *>(UINTPTR_MAX));
    auto it = timers_.lower_bound(sentry);
    assert(it == timers_.end() || now < it->first);
    //将红黑树里过期的元素拷贝到结果集
    std::copy(timers_.begin(), it, std::back_inserter(expired));
    timers_.erase(timers_.begin(), it);
    return expired;
}

void TimerQueue::addTimerInLoop(Timer *timer)
{
    loop_->assertInLoopThread();
    bool earliestChanged = insert(timer);
    if (earliestChanged)
    {
        resetTimerFd(timerfd_, timer->expiration());
    }
}

bool TimerQueue::insert(Timer *timer)
{
    bool earliestChanged = false;
    Timestamp when = timer->expiration();
    auto it = timers_.begin();
    if (it == timers_.end() || when < it->first)
        earliestChanged = true;

    auto pi = std::make_pair(when, timer);
    //pi.second->run();
    std::pair<TimerQueue::TimerList::iterator, bool>
        res = timers_.insert(pi);
    assert(res.second == true);
    return earliestChanged;
}
