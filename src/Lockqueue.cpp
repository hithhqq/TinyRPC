#include "Lockqueue.h"
template <typename T>
inline void LockQueue<T>::push(const T &data)
{
    std::lock_guard<std::mutex> lock(mutex_);
    queue_.push(data);
    condition_.notify_one();
}

template <typename T>
inline T &LockQueue<T>::pop()
{
    std::unique_lock<std::mutex> lock(mutex_);
    while (queue_.empty())
    {
        condition_.wait(lock);
    }

    T data = queue_.front();
    queue_.pop();
    return data;
}