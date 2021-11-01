#include "Log.h"






std::ostream &getLoggerHelper(int type,const char*file,const char*line)
{
    std::string level;
    if(type == 0)
        level = "Info";
    else if(type == 1)
        level = "Fatal";
    else level = "Warn";        

     std::cout << "[ "<<level<< "  tid: " << std::this_thread::get_id() << " file: " << file << "  line:" << line << " ] ";
        return std::cout;
}