#include <iostream>
#include <sstream>
#include <boost/any.hpp>
#include <map>

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <thread>
#include <chrono>
#include <condition_variable>
#include <mutex>


#include "swiftrobotc/swiftrobotc.h"
#include "swiftrobotc/msgs.h"
#include "swiftrobotc/usbhub.h"

std::mutex m;
std::condition_variable cv;

struct test_t {
    uint32_t test1;
    uint16_t test2;
    uint16_t test3;
    
    uint32_t serialize(char* dat) {
        memcpy(dat, &test1, 6);
        return 6;
    }
    
    static test_t deserialize(char* dat) {
        test_t msg;
        memcpy(&msg, dat, 6);
        return msg;
    }
};

static void hexdump(char* buf, int len) {
    for (int i = 0; i < len; ++i) {
        if ((uint8_t)buf[i] < 0x10)
            printf(" 0x0");
        else
            printf(" 0x");
        printf("%x", (uint8_t)buf[i]);
    }
    printf("\n");
}

int main(int argc, const char * argv[]) {
//// TEST MESSAGE TYPES
//
//    test_t msg;
//    msg.test1 = 0xbeef;
//    msg.test2 = 0x42;
//
//    char bytes[6];
//    msg.serialize(bytes);
//    hexdump(bytes, 6);
//
//    test_t msg2 = test_t::deserialize(bytes);
//    printf("-- %x %x \n", msg2.test1, msg2.test2);
//    printf("%lu\n", sizeof(test_t));

    SwiftRobotClient client(2345); // usbmux
    //SwiftRobotClient client("192.168.178.59", 2345); // wifi
    client.subscribe<internal_msg::UpdateMsg>(0, [](internal_msg::UpdateMsg msg) {
        printf("Device %d is now: %d \n", msg.deviceID, msg.status);
    });

    client.start();

    base_msg::UInt32Array msg_;
    std::vector<uint32_t> tmp;
    for (int i = 0; i< 100000; i++) { // 4 MB
        tmp.push_back(42);
    }
    msg_.data = tmp;

    client.subscribe<base_msg::UInt32Array>(26, [&](base_msg::UInt32Array msg) {
        printf("received tik took %d \n", msg.data[0]);
        cv.notify_one();
    });

    while(1) {
        std::unique_lock<std::mutex> l(m);
        cv.wait(l);
        client.publish(27, msg_);
    }
    
    
    
//    std::vector<char> data = {0x11, 0x22, 0x33};
//    while (1) {
//
//        sensor_msg::Image msg;
//        std::vector<uint8_t> tmp;
//        for (int i = 0; i< 10; i++) {
//            tmp.push_back(0xbe);
//        }
//        msg.width = 5;
//        msg.height = 2;
//        char format[] = "MONO";
//        memcpy(msg.pixelFormat, format, 4);
//        base_msg::UInt8Array pixelData;
//        pixelData.data = tmp;
//        pixelData.size = 10;
//        msg.pixelArray = pixelData;
//        client.publish<sensor_msg::Image>(2, msg);
//        printf("sent\n");
//
//        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
//    }
    return EXIT_SUCCESS;
}
