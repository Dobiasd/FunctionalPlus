// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <condition_variable>
#include <cstdint>
#include <deque>
#include <mutex>

namespace fplus
{

// A thread-safe queue.
template <typename T>
class queue
{
public:
    queue() :
        queue_(),
        mutex_(),
        cond_()
        {}
    fplus::maybe<T> pop()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        if (queue_.empty())
        {
            return {};
        }
        auto item = queue_.front();
        queue_.pop_front();
        return item;
    }

    void push(const T& item)
    {
        {
            std::unique_lock<std::mutex> lock(mutex_);
            queue_.push_back(item);
        }
        cond_.notify_one();
    }

    std::vector<T> pop_all()
    {
        std::unique_lock<std::mutex> mlock(mutex_);
        const auto result = fplus::convert_container<std::vector<T>>(queue_);
        queue_.clear();
        return result;
    }

    std::vector<T> wait_and_pop_all()
    {
        std::unique_lock<std::mutex> mlock(mutex_);
        cond_.wait(mlock, [&]() -> bool { return !queue_.empty(); });
        const auto result = fplus::convert_container<std::vector<T>>(queue_);
        queue_.clear();
        return result;
    }

    std::vector<T> wait_for_and_pop_all(std::int64_t max_wait_time_us)
    {
        std::unique_lock<std::mutex> mlock(mutex_);
        const auto t = std::chrono::microseconds{ max_wait_time_us };
        cond_.wait_for(mlock, t, [&]() -> bool { return !queue_.empty(); });
        const auto result = fplus::convert_container<std::vector<T>>(queue_);
        queue_.clear();
        return result;
    }

private:
    std::deque<T> queue_;
    std::mutex mutex_;
    std::condition_variable cond_;
};

} // namespace fplus
