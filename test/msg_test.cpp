#include <gtest/gtest.h>
#include <stdlib.h>
#include "swiftrobotc/msgs.h"
#include "swiftrobotc/swiftrobot_packet.h"

TEST(baseMsgTest, Serialize) {
    base_msg::UInt16Array msg;
    std::vector<uint16_t> vec = {0x00be, 0xffef};
    msg.data = vec;
    
    char bytes[8];
    msg.serialize(bytes);
    char exp_bytes[] = {0x04,0x00,0x00,0x00,(char)0xbe,0x00,(char)0xef,(char)0xff};
    EXPECT_TRUE(0 == std::memcmp( bytes, exp_bytes, 8));
}

TEST(baseMsgTest, Deserialize) {
    std::vector<uint16_t> vec = {0x00be, 0xffef};
    char msg_data[] = {0x04,0x00,0x00,0x00,(char)0xbe,0x00,(char)0xef,(char)0xff};
    base_msg::UInt16Array msg = base_msg::UInt16Array::deserialize(msg_data);
    EXPECT_EQ(msg.size, 4);
    for (char i = 0; i < msg.data.size(); ++i) {
      EXPECT_EQ(msg.data[i], vec[i]) << "Vectors x and y differ at index " << i;
    }
}

TEST(connectMsgHeader, Serialize) {
    message_connect_header_t msg;
    std::vector<uint16_t> vec = {0x00be, 0xffef};
    msg.clientid = "cl";
    msg.subscribers = 2;
    msg.channels = vec;
    
    EXPECT_EQ(msg.getSize(), 9);
    char bytes[9];
    msg.serialize(bytes);
    char exp_bytes[] = {99,108,0,2,0,(char)0xbe,0x00,(char)0xef,(char)0xff};
    EXPECT_TRUE(0 == std::memcmp( bytes, exp_bytes, 9));
}

TEST(connectMsgHeader, Deserialize) {
    std::vector<uint16_t> vec = {0x00be, 0xffef};
    char msg_data[] = {99,108,0,2,0,(char)0xbe,0x00,(char)0xef,(char)0xff};
    message_connect_header_t msg = message_connect_header_t::deserialize(msg_data);
    EXPECT_STREQ("cl", msg.clientid.c_str());
    EXPECT_EQ(msg.subscribers, 2);
    for (char i = 0; i < msg.channels.size(); ++i) {
      EXPECT_EQ(msg.channels[i], vec[i]) << "Vectors x and y differ at index " << i;
    }
}
