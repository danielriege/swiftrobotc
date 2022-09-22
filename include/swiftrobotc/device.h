#ifndef device_h
#define device_h

#include <stdio.h>
#include <stdlib.h>
#include <map>
#include <thread>
#include <chrono>

#include "swiftrobotc/socket_client.h"
#include "Plist.hpp"

#define USBMUX_KEY_MESSAGETYPE "MessageType"
#define USBMUX_KEY_NUMBER "Number"
#define USBMUX_KEY_CLIENTNAME "ProgName"
#define USBMUX_KEY_CLIENTVERSION "ClientVersionString"
#define USBMUX_KEY_PORT "PortNumber"
#define USBMUX_KEY_DEVICEID "DeviceID"

// Message types for usbmuxd
#define USBMUX_MESSAGETYPE_RESULT "Result"
#define USBMUX_MESSAGETYPE_ATTACH "Attached"
#define USBMUX_MESSAGETYPE_DETACH "Detached"
#define USBMUX_MESSAGETYPE_LISTEN "Listen"
#define USBMUX_MESSAGETYPE_CONNECT "Connect"

#define USBMUX_CLIENTNAME "swiftrobotc"
#define USBMUX_CLIENTVERSION "1.0"

#define USBMUX_TAG_BROADCAST 0
#define USBMUX_TAG_CONNECT 1
#define USBMUX_TAG_CLIENT 2

#define KEEPALIVE_TIMEOUT 5000 // milliseconds
#define KEEPALIVE_CHECK_TIMER 500 // milliseconds

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
/// Represents a device which is connected to a master. Can perform tasks for usbmuxd. Handles keep alive.
///
class Device {
private:
    SocketClient client;
    usb_device_info_t usb_info; // for USB
    std::string ip_address; // for WiFi
    device_status_t status;
    uint64_t port;
    uint32_t tag;
    bool usbmux; // if connection is usb or wifi
    std::chrono::time_point<std::chrono::system_clock> lastKeepAliveRequest;
    
    std::function<void(swiftrobot_packet_type_t type, char* data, size_t size)> packetReceivedCallback;
    std::function<void(uint8_t deviceID, uint8_t connected)> connectionStatusCallback;
    std::optional<std::thread> reconnectThread; // used to restart a connect if server is not started yet
    std::optional<std::thread> keepAliveCycleCheckThread;
    
public:
    Device(usb_device_info_t usb_device_info, uint16_t port); // for USB
    Device(std::string ip_address, uint16_t port); // for WiFi
    ~Device();
    
    void startConnection();
    void disconnect();
    
    device_status_t getStatus();
    
    void send(swiftrobot_packet_type_t type, char* data, size_t size);
    void send2(swiftrobot_packet_type_t type, char* data1, size_t size1, char* data2, size_t size2);
    void setPacketReceivedCallback(std::function<void(swiftrobot_packet_type_t type, char* data, size_t size)> callback);
    void setConnectionStatusCallback(std::function<void(uint8_t deviceID, uint8_t connected)> callback);
    
    static std::vector<char> createUSBMuxPacket(std::string message_type, std::map<std::string, boost::any> additionalPayload);
    static std::vector<char> createUSBMuxPacket(std::string message_type);
private:
    void connectRoutine(); // called after a successfull connection is made
    void messageReceived(swiftrobot_packet_header_t header, char*data, size_t size);
    void connectThread();
    void connect();
    // keep alive stuff
    bool checkKeepAliveTimeout();
    void startKeepAliveCheckCycle();
    void checkKeepAliveThread();
};

typedef std::shared_ptr<Device> DevicePtr;

#endif
