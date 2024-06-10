#pragma once
#include <queue>
#include <thread>  
#include <mutex>
#include <condition_variable>

template<typename T>
class LockQueue
{
public:
    void push(const T &data);
    T& pop();
private:
    std::queue<T> queue_;
    std::mutex mutex_;
    std::condition_variable condition_; 
};


