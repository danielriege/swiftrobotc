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


#include "swiftrobotc/swiftrobotc.h"
#include "swiftrobotc/msgs.h"
#include "swiftrobotc/usbhub.h"

int main(int argc, const char * argv[]) {
    SwiftRobotClient client(2345); // usbmux
    //SwiftRobotClient client("192.168.178.59", 2345); // wifi
    
    client.subscribe<internal_msg::UpdateMsg>(0, [](internal_msg::UpdateMsg msg) {
        printf("Device : %d is now: %d\n", msg.deviceID, msg.status);
    });
    
    client.start();
    
    client.subscribe<sensor_msg::IMU>(2, [](sensor_msg::IMU msg) {
        printf("recv imu data\n");
    });
    client.subscribe<sensor_msg::Image>(1, [](sensor_msg::Image msg) {
        //printf("image \n");
        //printf("width: %d height: %d pixelFormat %s \n", msg.width, msg.height, msg.pixelFormat);
    });
    
//    std::vector<char> data = {0x11, 0x22, 0x33};
    std::this_thread::sleep_for(std::chrono::milliseconds(8000));
    
    base_msg::UInt16Array msg;
    std::vector<uint16_t> tmp;
    for (int i = 0; i< 100000; i++) {
        tmp.push_back(0x00be);
    }
    msg.data = tmp;
    client.publish<base_msg::UInt16Array>(3, msg);
    printf("sent\n");
    
    while(1) {}
    
    
//    USBHub hub(2348);
//    hub.startLookingForConnections();
//    hub.registerReceiveCallback([](char*data, size_t size) {
//        std::string received_msg(data, size);
//        printf("received: %s\n", received_msg.c_str());
//        return 0;
//    });
//
//    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
//
//    std::string msg = "Hello world!!!";
//    hub.sendPacketToAll((char*)&msg[0], msg.length());
    
//    while (1) {}
    // create socket for tcp connection
//    int c_socket;
//    create_socket(&c_socket);
//    // send tcp connect request
//    uint16_t port = 2346;
//    port = ((port<<8) & 0xFF00) | (port>>8);
//    std::map<std::string, boost::any> connect_dict;
//    connect_dict["ClientVersionString"] = std::string("1");
//    connect_dict["DeviceID"] = int(deviceId);
//    connect_dict["MessageType"] = std::string("Connect");
//    connect_dict["PortNumber"] = int(port);
//    connect_dict["ProgName"] = std::string("Peertalk macOS Example");
//
//    std::vector<char> connect_msg;
//    Plist::writePlistXML(connect_msg, connect_dict);
//    std::string string2(connect_msg.begin(),connect_msg.end());
//    std::cout << (string2.data()) << std::endl;
//    send_msg((char*)&connect_msg[0], connect_msg.size(), c_socket);
//
//
//    // receive ACK
//    receive_msg(c_socket);
//
//    char* hello_msg = "hello world!\n";
//    send_msg(hello_msg, strlen(hello_msg), c_socket);
//
//    receive_msg(c_socket);
//
//    //t1.join();
//
//    close(l_socket);
    return EXIT_SUCCESS;
}
