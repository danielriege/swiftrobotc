#include "swiftrobotc/dispatch_queue.h"

dispatch_queue::dispatch_queue(size_t thread_cnt) :
    threads_(thread_cnt)
{
    for(size_t i = 0; i < threads_.size(); i++) {
        threads_[i] = std::thread(&dispatch_queue::dispatch_thread_handler, this);
    }
}

dispatch_queue::~dispatch_queue() {
    // Signal to dispatch threads that it's time to wrap up
    std::unique_lock<std::mutex> lock(lock_);
    quit_ = true;
    cv_.notify_all();
    lock.unlock();

    // Wait for threads to finish before we exit
    for(size_t i = 0; i < threads_.size(); i++) {
        if(threads_[i].joinable()) {
            threads_[i].join();
        }
    }
}

std::chrono::steady_clock::time_point dispatch_queue::now() {
    return std::chrono::steady_clock::now();
}

void dispatch_queue::dispatch(fnc function) {
    std::unique_lock<std::mutex> lock(lock_);
    q_.push(function);
    cv_.notify_one();
}

void dispatch_queue::dispatch_after(tp time_point, fnc function) {
    dispatch_item i{time_point, function};
    std::unique_lock<std::mutex> lock(lock_);
    pq_.push(i);
    cv_.notify_one();
}

void dispatch_queue::dispatch_thread_handler(void) {
    std::unique_lock<std::mutex> lock(lock_);

    do {
        // Wait until we have data or a quit signal
        cv_.wait(lock, [this] {
            return (q_.size() || pq_.size() || quit_);
        });

        // after wait, we own the lock
        if (!quit_ && pq_.size() && pq_.top().time_point < dispatch_queue::now()) {
            auto item = std::move(pq_.top());
            pq_.pop();
            lock.unlock();
            
            item.function();
            
            lock.lock();
        } else if(!quit_ && q_.size()) {
            auto function = std::move(q_.front());
            q_.pop();

            // unlock now that we're done messing with the queue
            lock.unlock();

            function();

            lock.lock();
        }
    } while(!quit_);
}
