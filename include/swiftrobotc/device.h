#ifndef device_h
#define device_h

#include <stdio.h>
#include <stdlib.h>
#include <map>
#include <thread>
#include <chrono>

#include "swiftrobotc/usbmux_client.h"
#include "Plist.hpp"

#define USBMUX_KEY_MESSAGETYPE "MessageType"
#define USBMUX_KEY_NUMBER "Number"
#define USBMUX_KEY_CLIENTNAME "ProgName"
#define USBMUX_KEY_CLIENTVERSION "ClientVersionString"
#define USBMUX_KEY_PORT "PortNumber"
#define USBMUX_KEY_DEVICEID "DeviceID"

// Message types
#define USBMUX_MESSAGETYPE_RESULT "Result"
#define USBMUX_MESSAGETYPE_ATTACH "Attached"
#define USBMUX_MESSAGETYPE_DETACH "Detached"
#define USBMUX_MESSAGETYPE_LISTEN "Listen"
#define USBMUX_MESSAGETYPE_CONNECT "Connect"

#define USBMUX_CLIENTNAME "swiftrobotc"
#define USBMUX_CLIENTVERSION "alpha"

#define USBMUX_TAG_BROADCAST 0
#define USBMUX_TAG_CONNECT 1
#define USBMUX_TAG_CLIENT 2


// reply codes
typedef enum usbmux_reply_code {
    USBMuxReplyCodeOK = 0,
    USBMuxReplyCodeBadCommand = 1,
    USBMuxReplyCodeBadDevice = 2,
    USBMuxReplyCodeConnectionRefused = 3,
    // ? = 4,
    USBMuxReplyCodeMalformedRequest = 5,
    USBMuxReplyCodeBadVersion = 6,
} usbmux_reply_code_t;

typedef struct usb_device_info {
    uint32_t device_id;
    uint32_t location_id;
    uint32_t product_id;
    uint64_t connection_speed;
    std::string connection_type;
    std::string serial_number;
    std::string udid;
    std::string usb_serial_number;
} usb_device_info_t;

typedef enum device_status {
    IDLE,
    WANTING_CONNECTION,
    CONNECTING,
    CONNECTED,
} device_status_t;

///
/// Manages every message to and from a specific iOS device. One instance per connected device.
///
class Device {
private:
    SocketClient client;
    usb_device_info_t usb_info; // for USB
    std::string ip_address; // for WiFi
    device_status_t status;
    uint64_t port;
    uint32_t tag;
    bool usbmux;
    
    std::function<void(char* data, size_t size)> outgoingMessage;
    std::function<void(uint8_t deviceID, uint8_t connected)> connectedCallback;
    std::thread reconnectThread;
    
public:
    Device(usb_device_info_t usb_device_info, uint16_t port); // for USB
    Device(std::string ip_address, uint16_t port); // for WiFi
    ~Device();
    
    void startConnection();
    void disconnect();
    
    device_status_t getStatus();
    
    void send(char* data, size_t size);
    void setIncomingCallback(std::function<void(char* data, size_t size)> callback);
    void setConnectedCallback(std::function<void(uint8_t deviceID, uint8_t connected)> callback);
    
    static std::vector<char> createUSBMuxPacket(std::string message_type, std::map<std::string, boost::any> additionalPayload);
    static std::vector<char> createUSBMuxPacket(std::string message_type);
private:
    void messageReceived(usbmux_header_t header, char*data, size_t size);
    void connectThread();
    void connect();
};

typedef std::shared_ptr<Device> DevicePtr;

#endif
