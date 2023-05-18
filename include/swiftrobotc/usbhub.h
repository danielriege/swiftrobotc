#ifndef Channel_h
#define Channel_h

#include <stdio.h>
#include <vector>
#include <exception>
#include <string>

#include "socket_client.h"
#include "connection.h"
#include "dispatch_queue.h"
#include "Plist.hpp"

#define DEVICE_STATUS_ATTACHED 0
#define DEVICE_STATUS_DETACHED 1
#define DEVICE_STATUS_CONNECTED 2
#define DEVICE_STATUS_DISCONNECTED 3

///
/// Managed usb connected iOS devices using usbmuxd protocol
///
class USBHub {
private:
    DispatchQueuePtr queue;
    /// USBHub indexes connections via an ID from usbmux
    std::map<int,ConnectionPtr> devices;
    SocketClient broadcastClient;
    uint16_t port;
    std::function<void(std::string clientID, swiftrobot_packet_type_t type, char* data, size_t size)> receivedPacketCallback;
    std::function<void(int deviceID, uint8_t status)> deviceStatusCallback;
public:
    USBHub(uint16_t port, DispatchQueuePtr queue);
    
    void startLookingForConnections();
    void close();
    
    void sendPacket(swiftrobot_packet_type_t type, std::string clientID, char* data, size_t size);
    /// Use this only if cliendID is unkown, like when sending connect message
    void sendPacket(swiftrobot_packet_type_t type, int deviceID, char* data, size_t size);
    void sendPacketToAll(swiftrobot_packet_type_t type, char* data, size_t size);
    void send2Packet(swiftrobot_packet_type_t type, std::string clientID, char* data1, size_t size1, char* data2, size_t size2);
    void send2PacketToAll(swiftrobot_packet_type_t type, char* data1, size_t size1, char* data2, size_t size2);
    void registerReceiveCallback(std::function<void(std::string, swiftrobot_packet_type_t type, char* data, size_t size)> callback);
    void registerStatusUpdateCallback(std::function<void(int deviceID, uint8_t status)> callback);
    
private:
    void broadcastHandler(swiftrobot_packet_header_t header, char*data, size_t size);
    void handleResult(usbmux_reply_code_t reply_code, swiftrobot_packet_header_t msg_header);
    void createDevice(usb_device_info_t device, uint16_t port);
    void removeDevice(int deviceID);
    int getDeviceIDForClientID(std::string clientid);
    
    static usb_device_info_t parsePropertiesPlistDict(std::map<std::string, boost::any> plist);
};

#endif
