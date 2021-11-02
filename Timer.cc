#include "Timer.h"

Timer::Timer(TimerCallback cb,int interval,Timestamp expiration)
:cb_(cb),
interval_(interval),
expiration_(expiration),
repeated_(interval_ >0.0)
{}
Timer::~Timer(){}

void Timer::restart(Timestamp now){
    if(repeated_)
        expiration_ = addTime(now,interval_);
    else expiration_ = Timestamp();    
}