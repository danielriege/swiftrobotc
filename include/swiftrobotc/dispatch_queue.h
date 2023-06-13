#pragma once

#include <thread>
#include <functional>
#include <vector>
#include <cstdint>
#include <cstdio>
#include <queue>
#include <mutex>
#include <string>
#include <condition_variable>
#include <chrono>

class dispatch_queue {
public:
    using tp = std::chrono::steady_clock::time_point;
    using fnc = std::function<void()>;

    struct dispatch_item {
        tp time_point;
        fnc function;

        bool operator<(const dispatch_item& other) const {
            return time_point > other.time_point;  // Use > to make it a min-heap (earliest time point on top)
        }
    };
    
    dispatch_queue(size_t thread_cnt = 1);
    ~dispatch_queue();

    // dispatch and move
    void dispatch(fnc function);
    
    void dispatch_after(tp time_point, fnc function);
    
    static tp now();
    
    // Deleted operations
    dispatch_queue(const dispatch_queue& rhs) = delete;
    dispatch_queue& operator=(const dispatch_queue& rhs) = delete;
    dispatch_queue(dispatch_queue&& rhs) = delete;
    dispatch_queue& operator=(dispatch_queue&& rhs) = delete;
    
private:
    std::mutex lock_;
    std::vector<std::thread> threads_;
    std::priority_queue<dispatch_item> pq_;
    std::queue<std::function<void()>> q_;
    std::condition_variable cv_;
    bool quit_ = false;

    void dispatch_thread_handler(void);
};

typedef std::shared_ptr<dispatch_queue> DispatchQueuePtr;
