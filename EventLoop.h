#pragma once
#include <unistd.h>
#include <memory>
#include <vector>
#include <mutex>
#include "Log.h"
#include "Channel.h"
#include "Timestamp.h"
#include "TimeQueue.h"
#include "Type.h"
class Epoller;
class EventLoop : nocopyable
{

public:
    typedef std::function<void()> Functor;

public:
    EventLoop();
    ~EventLoop();
    void updateChannel(Channel *c);
    void removeChannel(Channel *c);

    void quit();
    void loop();
    //检查当前执行代码的线程是否是EventLoop的主人
    void assertInLoopThread()
    {
       //LOG<<"loopId = "<<threadId_<<"  "<<ENDL;
        if (!isInLoopThread())
            abortNotInLoopThread();
    };

    bool isInLoopThread() const
    {
        //LOG<<threadId_<<" "<<std::this_thread::get_id()<<ENDL;
        return threadId_ == std::this_thread::get_id();
    }

    TimerId runAt(const Timestamp &time, const TimerCallback &cb)
    {
        return timerQueue_->addTimer(cb, time, 0.0);
    }
    TimerId runAfter(double delay, const TimerCallback &cb)
    {
        return runAt(addTime(Timestamp::now(), delay), cb);
    };
    TimerId runEvery(double interval, const TimerCallback &cb)
    {
        return timerQueue_->addTimer(cb, Timestamp::now(), interval);
    }
    void runInLoop(const Functor &cb);
    static EventLoop *getEventLoopInCurrentThread();
    void wakeup();
    void handelRead();
    void doPendingFunctors();
    void queueInLoop(const Functor &cb);

private:
    typedef std::vector<Channel *> ChannelList;
    static const int kPollTimeMs = 10000;
    const std::thread::id threadId_; //eventLoop 的主人线程的id
    bool looping_;
    bool quit_;
    bool callPendingFunctors_;
    int wakeupFd_;
    Timestamp pollReturnTime;
    ChannelList activeChannels_;
    std::unique_ptr<Epoller> poller_;
    std::unique_ptr<TimerQueue> timerQueue_;
    std::unique_ptr<Channel> wakeupChannel_;
    std::mutex mutex_;
    std::vector<Functor> pendingFunctors_;

private:
    void abortNotInLoopThread()
    {
        LOG << "abort !" << ENDL;
        abort();
    };
};