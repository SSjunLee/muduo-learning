#pragma once
#include <unistd.h>
#include <memory>
#include <vector>
#include <thread>
#include "Log.h"
#include "Channel.h"
#include "Timestamp.h"
class Epoller;
class EventLoop
{
public:
    EventLoop();
    ~EventLoop();
    void updateChannel(Channel *c);
    void quit();
    void loop();
    //每个线程最多有一个Eventloop,确定此时运行的eventloop对象是在创建它的线程里
    void assertInLoopThread()
    {
        if (!isInLoopThread())
            abortNotInLoopThread();
    };

    bool isInLoopThread() const { return threadId_ == std::this_thread::get_id(); }

    static EventLoop *getEventLoopInCurrentThread();

private:
    typedef std::vector<Channel *> ChannelList;
    ChannelList activeChannels_;
    std::unique_ptr<Epoller> poller_;
    bool looping_;
    bool quit_;
    Timestamp pollReturnTime;
    const std::thread::id threadId_;
    static const int kPollTimeMs = 10000;


private:
     void abortNotInLoopThread()
    {
        LOG << "abort !" << ENDL;
    };

};