#ifndef Channel_h
#define Channel_h

#include <stdio.h>
#include <vector>
#include <exception>

#include "usbmux_client.h"
#include "device.h"
#include "Plist.hpp"

#define DEVICE_STATUS_ATTACHED 0
#define DEVICE_STATUS_DETACHED 1
#define DEVICE_STATUS_CONNECTED 2

///
/// Manages all iOS devices connected via USB
///
class USBHub {
private:
    std::map<uint32_t,DevicePtr> devices;
    USBMuxClient broadcastClient;
    uint16_t port;
    std::function<void(char* data, size_t size)> receivedPacketCallback;
    std::function<void(uint8_t deviceID, uint8_t status)> deviceStatusCallback;
public:
    USBHub(uint16_t port);
    
    void startLookingForConnections();
    void close();
    
    void sendPacket(int deviceID, char* data, size_t size);
    void sendPacketToAll(char* data, size_t size);
    void registerReceiveCallback(std::function<void(char* data, size_t size)> callback);
    void registerStatusUpdateCallback(std::function<void(uint8_t deviceID, uint8_t status)>);
private:
    void broadcastHandler(usbmux_header_t header, char*data, size_t size);
    void handleResult(usbmux_reply_code_t reply_code, usbmux_header_t msg_header);
    void createDevice(device_info_t device, uint16_t port);
    void removeDevice(int deviceID);
    
    static device_info_t parsePropertiesPlistDict(std::map<std::string, boost::any> plist);
};

#endif
