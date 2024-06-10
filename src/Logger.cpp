#include "Logger.h"

Logger& Logger::GetInstance()
{
    static Logger logger_;
    return logger_;
}

Logger::Logger()
{
    
}
void Logger::SetLogLevel(LogLevel level)
{
    loglevel_ = level;
}

// 把日志信息写入lockqueue缓冲区
void Logger::Log(std::string msg)
{
    lockqueue_.push(msg);
}

