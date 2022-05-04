//#ifndef wifihub_h
//#define wifihub_h
//
//#include <stdio.h>
//#include <vector>
//#include <exception>
//
//#include "usbmux_client.h"
//#include "device.h"
//#include "Plist.hpp"
//
//#define DEVICE_STATUS_DETACHED 1
//#define DEVICE_STATUS_CONNECTED 2
//
/////
///// Manages all iOS devices connected via USB
/////
//class WiFiClient {
//private:
//    SocketClient client;
//    uint16_t port;
//    std::string ip_address;
//    std::function<void(char* data, size_t size)> receivedPacketCallback; // identical to USBHub
//    std::function<void(uint8_t deviceID, uint8_t status)> deviceStatusCallback; // identical to USBHub
//public:
//    WiFiClient(std::string ip, uint16_t port);
//
//    void connect();
//    void close();
//
//    void sendPacket(char* data, size_t size);
//    void registerReceiveCallback(std::function<void(char* data, size_t size)> callback);
//    void registerStatusUpdateCallback(std::function<void(uint8_t deviceID, uint8_t status)>);
//private:
//    void broadcastHandler(usbmux_header_t header, char*data, size_t size);
//    void handleResult(usbmux_reply_code_t reply_code, usbmux_header_t msg_header);
//    void createDevice(device_info_t device, uint16_t port);
//    void removeDevice(int deviceID);
//};
//
//#endif
//
