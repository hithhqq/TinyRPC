#pragma once
#include "Lockqueue.h"
enum LogLevel
{
    INFO,
    ERROR,
};

class Logger
{
public:
    static Logger& GetInstance();
    void SetLogLevel(LogLevel level);
    void Log(std::string msg);
private:
    int loglevel_;
    LockQueue<std::string> lockqueue_;
    Logger();
    Logger(const Logger&) = delete;
};