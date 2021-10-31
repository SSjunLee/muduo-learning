#pragma once
#include<iostream>
#include<thread>
#define LOG getLogger()
#define ENDL std::endl 
inline std::ostream& getLogger(){
    std::cout<<"[ tid: "<<std::this_thread::get_id()<<" file: "<<__FILE__<<"  line:"<<__LINE__<<" ] ";
    return std::cout;
}

