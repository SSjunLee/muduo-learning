#pragma once
#include<unistd.h>
#include<thread>
#include "Log.h"
#include "Channel.h"
class EventLoop{
public:
    EventLoop();
    ~EventLoop();
    void updateChannel(Channel* c);

    void loop();
    //每个线程最多有一个Eventloop,确定此时运行的eventloop对象是在创建它的线程里
    void assertInLoopThread()
    {
        if(!isInLoopThread())
            abortNotInLoopThread();
    };

    bool isInLoopThread() const{return threadId_ == std::this_thread::get_id();}

    static EventLoop* getEventLoopInCurrentThread();

private:
    void abortNotInLoopThread(){
        LOG<<"abort !"<<ENDL;
    };

    bool looping_;
    const std::thread::id threadId_;


};