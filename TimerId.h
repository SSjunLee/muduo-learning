#pragma once
class Timer;
class TimerId
{
public:
    explicit TimerId(Timer *timer = NULL) :
     timer_(timer)
    {
    }

private:
    Timer *timer_;
};
