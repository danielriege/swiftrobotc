#include <gtest/gtest.h>
#include <stdlib.h>
#include "swiftrobotc/dispatch_queue.h"

using namespace std::chrono;

template<typename Q>
void print_queue(Q q)
{
    // NB: q is passed by value because there is no way to traverse
    // priority_queue's content without erasing the queue.
    for (; !q.empty(); q.pop()) {
        time_point<steady_clock, milliseconds> time_point;
        time_point = time_point_cast<milliseconds>(q.top().time_point);
        printf("%d\n", time_point);
    }
}

TEST(dispatch_queue, dispatchOrderDeadline) {
    std::atomic_bool callback_flag_1{false};
    std::atomic_bool callback_flag_2{false};
    std::atomic_bool callback_flag_3{false};
    
    dispatch_queue q;
    
    q.dispatch_after(dispatch_queue::now() + seconds(2), [&] {
        callback_flag_2 = true;
    });
    
    q.dispatch_after(dispatch_queue::now() + seconds(4), [&] {
        callback_flag_3 = true;
    });
    
    q.dispatch_after(dispatch_queue::now() + seconds(1), [&] {
        callback_flag_1 = true;
    });
    
    std::this_thread::sleep_for(std::chrono::milliseconds(900));
    EXPECT_FALSE(callback_flag_1);
    EXPECT_FALSE(callback_flag_2);
    EXPECT_FALSE(callback_flag_3);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    EXPECT_TRUE(callback_flag_1);
    EXPECT_FALSE(callback_flag_2);
    EXPECT_FALSE(callback_flag_3);
    std::this_thread::sleep_for(std::chrono::milliseconds(800));
    EXPECT_TRUE(callback_flag_1);
    EXPECT_FALSE(callback_flag_2);
    EXPECT_FALSE(callback_flag_3);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    EXPECT_TRUE(callback_flag_1);
    EXPECT_TRUE(callback_flag_2);
    EXPECT_FALSE(callback_flag_3);
    std::this_thread::sleep_for(std::chrono::milliseconds(1800));
    EXPECT_TRUE(callback_flag_1);
    EXPECT_TRUE(callback_flag_2);
    EXPECT_FALSE(callback_flag_3);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    EXPECT_TRUE(callback_flag_1);
    EXPECT_TRUE(callback_flag_2);
    EXPECT_TRUE(callback_flag_3);
}

TEST(dispatch_queue, dispatchOrderMixed) {
    std::atomic_bool callback_flag_1{false};
    std::atomic_bool callback_flag_2{false};
    std::atomic_bool callback_flag_3{false};
    
    dispatch_queue q;
    
    q.dispatch_after(dispatch_queue::now() + seconds(2), [&] {
        callback_flag_3 = true;
    });
    
    q.dispatch([&] {
        callback_flag_1 = true;
    });
    
    q.dispatch_after(dispatch_queue::now() + seconds(1), [&] {
        callback_flag_2 = true;
    });
    
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    EXPECT_TRUE(callback_flag_1);
    EXPECT_FALSE(callback_flag_2);
    EXPECT_FALSE(callback_flag_3);
    std::this_thread::sleep_for(std::chrono::milliseconds(900));
    EXPECT_TRUE(callback_flag_1);
    EXPECT_FALSE(callback_flag_2);
    EXPECT_FALSE(callback_flag_3);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    EXPECT_TRUE(callback_flag_1);
    EXPECT_TRUE(callback_flag_2);
    EXPECT_FALSE(callback_flag_3);
    std::this_thread::sleep_for(std::chrono::milliseconds(800));
    EXPECT_TRUE(callback_flag_1);
    EXPECT_TRUE(callback_flag_2);
    EXPECT_FALSE(callback_flag_3);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    EXPECT_TRUE(callback_flag_1);
    EXPECT_TRUE(callback_flag_2);
    EXPECT_TRUE(callback_flag_3);
}

TEST(dispatch_queue, dispatchOrderMixed2) {
    std::atomic_bool callback_flag_1{false};
    std::atomic_bool callback_flag_2{false};
    std::atomic_bool callback_flag_3{false};
    
    dispatch_queue q;
    
    q.dispatch_after(dispatch_queue::now() + seconds(2), [&] {
        callback_flag_3 = true;
    });
    
    q.dispatch_after(dispatch_queue::now() + seconds(1), [&] {
        callback_flag_1 = true;
        q.dispatch([&] {
            callback_flag_2 = true;
        });
    });
    
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    EXPECT_FALSE(callback_flag_1);
    EXPECT_FALSE(callback_flag_2);
    EXPECT_FALSE(callback_flag_3);
    std::this_thread::sleep_for(std::chrono::milliseconds(900));
    EXPECT_FALSE(callback_flag_1);
    EXPECT_FALSE(callback_flag_2);
    EXPECT_FALSE(callback_flag_3);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    EXPECT_TRUE(callback_flag_1);
    EXPECT_TRUE(callback_flag_2);
    EXPECT_FALSE(callback_flag_3);
    std::this_thread::sleep_for(std::chrono::milliseconds(800));
    EXPECT_TRUE(callback_flag_1);
    EXPECT_TRUE(callback_flag_2);
    EXPECT_FALSE(callback_flag_3);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    EXPECT_TRUE(callback_flag_1);
    EXPECT_TRUE(callback_flag_2);
    EXPECT_TRUE(callback_flag_3);
}
