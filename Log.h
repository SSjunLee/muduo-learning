#pragma once
#include <iostream>
#include <thread>
#define __LOG__(type,file,line) getLogger(type,file,line)
#define LOG __LOG__(0,__FILE__,__LINE__)
#define LOG_SYSFATAL __LOG__(1,__FILE__,__LINE__)
#define LOG_WARN __LOG__(2,__FILE__,__LINE__)



#define ENDL std::endl

std::ostream &getLoggerHelper(int type,const char*file,const char*line);
inline std::ostream &getLogger(int type,const char*file,int line)
{
  const char*l =std::to_string(line).c_str() ;
   return getLoggerHelper(type,file,l);
}


