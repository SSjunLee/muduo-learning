#pragma once
#include<iostream>
#include<unistd.h>
#include<sys/types.h>
class Timestamp
{
public:
    static const int kMicroSecondPerSecond = 1000*1000;

public:
    bool valid() const {return microSecondsSinceEpoch_>0;}
    Timestamp():microSecondsSinceEpoch_(0){};
    explicit Timestamp(int64_t microSecondsSinceEpoch)
    :microSecondsSinceEpoch_(microSecondsSinceEpoch)
    { }
    void swap(Timestamp&that){
        std::swap(microSecondsSinceEpoch_,that.microSecondsSinceEpoch_);
    }
    std::string toString() const;
    ~Timestamp();
    int64_t microSecondsSinceEpoch() const {return microSecondsSinceEpoch_;}

    static Timestamp now();
private:
    int64_t microSecondsSinceEpoch_;
};


inline bool  operator<(Timestamp l,Timestamp r){
     return l.microSecondsSinceEpoch() < r.microSecondsSinceEpoch();
};

inline bool  operator == (Timestamp l,Timestamp r){
     return l.microSecondsSinceEpoch() == r.microSecondsSinceEpoch();
}

inline Timestamp addTime(Timestamp timestamp,double secounds)
{
    int64_t delta = static_cast<int64_t>(secounds* Timestamp::kMicroSecondPerSecond);
    return Timestamp(timestamp.microSecondsSinceEpoch() + delta);
}








