#include <gtest/gtest.h>
#include <stdlib.h>
#include <thread>
#include "swiftrobotc/msgs.h"
#include "swiftrobotc/swiftrobotc.h"

TEST(oneClient, publishForget) {
    std::atomic_bool callback_called_flag{false};
    
    SwiftRobotClient client(connection_type_t::LOCAL_ONLY, "clientA");
    base_msg::UInt8Array msg;
    msg.data = std::vector<uint8_t>{0x01,0x02};
    client.publish(0x01, msg);
    
    client.subscribe<base_msg::UInt8Array>(0x01, [&callback_called_flag](base_msg::UInt8Array msg) {
        callback_called_flag = true;
    });
    
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    ASSERT_FALSE(callback_called_flag);
}

TEST(oneClient, publishRight) {
    std::atomic_bool callback_called_flag{false};
    
    SwiftRobotClient client(connection_type_t::LOCAL_ONLY, "clientA");
    
    client.subscribe<base_msg::UInt8Array>(0x01, [&callback_called_flag](base_msg::UInt8Array msg) {
        callback_called_flag = true;
    });
    
    base_msg::UInt8Array msg;
    msg.data = std::vector<uint8_t>{0x01,0x02};
    client.publish(0x01, msg);
    
    int retry_count = 100;
    while (!callback_called_flag && retry_count-- > 0)
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    ASSERT_TRUE(callback_called_flag);
}

TEST(oneClient, subscribeOverload) {
    std::atomic_bool callback_called_flag{false};
    std::atomic_bool callback_called_flag2{false};
    
    SwiftRobotClient client(connection_type_t::LOCAL_ONLY, "clientA");
    
    client.subscribe<base_msg::UInt8Array>(0x01, [&](base_msg::UInt8Array msg) {
        // simulate heavy callback
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        if (msg.data[0] == 1) {
            callback_called_flag = true;
        } else {
            callback_called_flag2 = true;
        }
    });
    
    base_msg::UInt8Array msg;
    msg.data = std::vector<uint8_t>{0x01};
    client.publish(0x01, msg);
    
    base_msg::UInt8Array msg2;
    msg2.data = std::vector<uint8_t>{0x02};
    client.publish(0x01, msg2);
    
    std::this_thread::sleep_for(std::chrono::milliseconds(250));
    ASSERT_TRUE(callback_called_flag);
    ASSERT_FALSE(callback_called_flag2);
}
