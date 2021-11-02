#pragma once
#include "Type.h"
#include "Timestamp.h"

class Timer:nocopyable
{

public:
    Timer(TimerCallback cb,int interval,Timestamp expiration);
    ~Timer();
    void run() const {
        cb_();
    }
    Timestamp expiration() const {return expiration_;}
    bool repeat() const {return repeated_;}
    void restart(Timestamp now);

private:
    int interval_;
    bool repeated_;
    TimerCallback cb_;
    Timestamp expiration_;

};
