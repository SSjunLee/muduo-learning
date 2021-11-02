#include<iostream>
#include<thread>
#include<unistd.h>
#include<sys/timerfd.h>
#include<cstring>
#include"Log.h"
#include"EventLoop.h"

EventLoop*g_loop;
void timeOut(){
    std::cout<<"time out"<<std::endl;
    g_loop->quit();
}
int main(){
    EventLoop loop;
    g_loop = &loop;

    int tfd =    timerfd_create(CLOCK_MONOTONIC,TFD_NONBLOCK|TFD_CLOEXEC);   
    Channel channel(g_loop,tfd);
    channel.setReadCallback_(timeOut);
    channel.enableReading();

    struct itimerspec howlong;
    bzero(&howlong,sizeof(struct itimerspec));
    howlong.it_value.tv_sec = 5;
    timerfd_settime(tfd,0,&howlong,NULL);

    loop.loop();
    close(tfd);
}