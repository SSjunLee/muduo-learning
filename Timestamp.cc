#include<sys/time.h>
#include<stdlib.h>
#include<stdio.h>
#include "Timestamp.h"


std::string Timestamp::toString() const{
    char buf[32] = {0};
    int64_t seconds = microSecondsSinceEpoch_/kMicroSecondPerSecond;
    int64_t microseconds = microSecondsSinceEpoch_ % kMicroSecondPerSecond;
    snprintf(buf,sizeof(buf),"%d.%06d",seconds,microseconds);
    return buf;
}

Timestamp Timestamp::now() {
    struct timeval tv;
    gettimeofday(&tv,NULL);
    int64_t seconds = tv.tv_sec;
    return Timestamp(seconds*kMicroSecondPerSecond + tv.tv_usec);
}



Timestamp::~Timestamp()
{
}
