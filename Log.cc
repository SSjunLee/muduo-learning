#include "Log.h"
#include "string.h"

__thread char t_errorbuf[512];



const char* debugs::geterror(int no)
{
    return strerror_r(no,t_errorbuf,sizeof t_errorbuf);
}



std::ostream &getLoggerHelper(int type,const char*file,const char*line)
{
    std::string level;
    if(type == 0)
        level = "Info";
    else if(type == 1)
        level = "Warn";
    else if(type == 2) level = "Error";        
    else if(type == 3) level = "Fatal";
     std::cout << "[ "<<level<< "  tid: " << std::this_thread::get_id() << " file: " << file << "  line:" << line << " ] ";
        return std::cout;
}