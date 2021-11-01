#pragma once
#include<iostream>
#include<unistd.h>
#include<sys/types.h>
class Timestamp
{

public:
    Timestamp():microSecondsSinceEpoch_(0){};
    explicit Timestamp(int64_t microSecondsSinceEpoch)
    :microSecondsSinceEpoch_(microSecondsSinceEpoch)
    { }
    void swap(Timestamp&that){
        std::swap(microSecondsSinceEpoch_,that.microSecondsSinceEpoch_);
    }
    std::string toString() const;
    ~Timestamp();

    static Timestamp now();
private:
    static const int kMicroSecondPerSecond = 1000*1000;
    int64_t microSecondsSinceEpoch_;
};

