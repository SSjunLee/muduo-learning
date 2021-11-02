#pragma once
#include<functional>
#include"nocopyable.h"
class EventLoop;
class Channel:nocopyable
{
    
public:
    typedef std::function<void()> EventCallback;
    Channel(EventLoop* loop, int fd);

    void handleEvent();
    void setReadCallback_(const EventCallback&cb){readCallback_ = cb;}
    void setWriteCallback(const EventCallback&cb){writeCallback_ = cb;}
    void setErrorCallback(const EventCallback&cb){errorCallback_ = cb;}
    
    int fd() const {return fd_;}
    int events() const {return events_;}
    void set_revents(int revents){revents_ = revents;}
    bool isNoneEvent() const { return events_ == kNoneEvent;}

    void enableReading(){events_|=kReadEvent;update();}
    void enableWriting(){events_|=kWriteEvent;update();}
    void disableReading(){events_&=~kReadEvent;update();}
    void disableWriting(){events_&=~kWriteEvent;update();}

    int index(){return index_;}
    void set_index(int idx){index_ = idx;}
    EventLoop* ownerLoop(){return loop_;}
    std::string eventsToString(int fd,int ev);


private:

    void update();

    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;


    EventLoop* loop_;
    const int fd_;
    int events_; //用户关心的事件
    int revents_;//激活的事件
    int index_;    

    EventCallback readCallback_;
    EventCallback writeCallback_;
    EventCallback errorCallback_;
};


