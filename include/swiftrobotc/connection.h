#ifndef device_h
#define device_h

#include <stdio.h>
#include <stdlib.h>
#include <map>
#include <thread>
#include <chrono>
#include <string>

#include "swiftrobotc/socket_client.h"
#include "Plist.hpp"
#include "swiftrobotc/swiftrobot_packet.h"
#include "swiftrobotc/dispatch_queue.h"

typedef enum connection_status {
    IDLE,
    WANTING_CONNECTION,
    CONNECTING,
    CONNECTED,
} connection_status_t;

///
///  Representation of a low level swiftrobot connection. Can be created using an IP address or with usb device info.
///  In case of USB connection, it will handle usbmux protocol. Methods can be used independent from connection type.
///
class Connection {
private:
    DispatchQueuePtr queue;
    SocketClient client;
    usb_device_info_t usb_info; // for USB
    std::string ip_address; // for WiFi
    connection_status_t status;
    uint64_t port;
    uint32_t tag;
    bool usbmux; // if connection is usb or wifi
    //std::chrono::time_point<std::chrono::system_clock> lastKeepAliveRequest;
    std::string name;
    
    std::function<void(std::string name, swiftrobot_packet_type_t type, char* data, size_t size)> packetReceivedCallback;
    std::function<void(std::string name, uint8_t connected)> connectionStatusCallback;
   // std::optional<std::thread> keepAliveCycleCheckThread;
    
public:
    Connection(usb_device_info_t usb_device_info, uint16_t port, DispatchQueuePtr queue); // for USB
    Connection(std::string name, std::string ip_address, uint16_t port, DispatchQueuePtr queue); // deprecated
    ~Connection();
    
    void startConnection();
    void disconnect();
    
    connection_status_t getStatus();
    std::string getName();
    
    void send(swiftrobot_packet_type_t type, char* data, size_t size);
    void send2(swiftrobot_packet_type_t type, char* data1, size_t size1, char* data2, size_t size2);
    void setPacketReceivedCallback(std::function<void(std::string name, swiftrobot_packet_type_t type, char* data, size_t size)> callback);
    void setConnectionStatusCallback(std::function<void(std::string name, uint8_t connected)> callback);
    
    static std::vector<char> createUSBMuxPacket(std::string message_type, std::map<std::string, boost::any> additionalPayload);
    static std::vector<char> createUSBMuxPacket(std::string message_type);
private:
    void connectedRoutine(); // called after a successfull connection is made
    void messageReceived(swiftrobot_packet_header_t header, char*data, size_t size);
    void connectThread();
    void connect();
};

typedef std::shared_ptr<Connection> ConnectionPtr;

#endif
