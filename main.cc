#include<iostream>
#include<thread>
#include<unistd.h>
#include"Log.h"
#include"EventLoop.h"

EventLoop loop;
void fuc(){
    printf("thread() pid = %d,tid = %d\n",getpid(),std::this_thread::get_id());
    loop.loop();
}

int main(){
    printf("main() pid = %d,tid = %d\n",getpid(),std::this_thread::get_id());
    
    
    std::thread t(fuc);
    loop.loop();
    t.join();
}