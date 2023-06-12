#pragma once

#include <string>
#include <cstring>

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

typedef struct message_connect_header {
    std::string clientid;
    uint16_t subscribers;
    std::vector<uint16_t> channels;
    
    void serialize(char* dat) {
        memcpy(dat, clientid.c_str(), clientid.size() + 1);
        memcpy(dat + clientid.size() + 1, &subscribers, sizeof(uint16_t));
        memcpy(dat + clientid.size() + 1 + sizeof(uint16_t), &channels[0], subscribers*sizeof(uint16_t));
    }
    
    static message_connect_header deserialize(char* data) {
        message_connect_header connect_msg;
        connect_msg.clientid = std::string(data);
        std::size_t string_length = std::strlen(data);
        const uint16_t* ptr_subscribers = reinterpret_cast<const uint16_t*>(data + string_length + 1);
        connect_msg.subscribers = *ptr_subscribers;
        
        connect_msg.channels = std::vector<uint16_t>(connect_msg.subscribers);
        memcpy(&connect_msg.channels[0], data+string_length+1+sizeof(uint16_t), connect_msg.subscribers * sizeof(uint16_t));
        
        return connect_msg;
    }
    
    size_t getSize() {
        return clientid.length() + 1 + sizeof(subscribers) + channels.size() * sizeof(uint16_t);
    }
    
} message_connect_header_t;

typedef struct message_packet_header {
    uint16_t channel;
    uint16_t type;
    uint32_t data_size;
} message_packet_header_t;

typedef struct subscribe_request_packet_header {
    uint16_t channel;
} subscribe_request_packet_header_t;

typedef enum connection_type {
    USB,
    WIFI,
    LOCAL_ONLY
} connection_type_t;

///
/// usbmuxd header
///
typedef struct swiftrobot_packet_header {
    uint32_t length;
    uint32_t protocol;
    uint32_t type;
    uint32_t tag;
} swiftrobot_packet_header_t;

typedef struct swiftrobot_packet {
    swiftrobot_packet_header_t header;
    char* payload;
} swiftrobot_packet_t;

typedef enum swiftrobot_packet_type {
    // Deprecated USBMux types
    USBMuxPacketTypeResult = 1,
    USBMuxPacketTypeConnect = 2,
    USBMuxPacketTypeListen = 3,
    USBMuxPacketTypeDeviceAdd = 4,
    USBMuxPacketTypeDeviceRemove = 5,
    // ? = 6,
    // ? = 7,
    USBMuxPacketTypePlistPayload = 8, // only supported type by usbmuxd by now
    // Custom Types
    SwiftRobotPacketTypeMessage = 9,
    /// subscribe request to let the peer know what channels should be distributed
    SwiftRobotPacketTypeSubscribeRequest = 10,
    /// check if peer is still alive
    SwiftRobotPacketTypeKeepAliveRequest = 11,
    /// Reply to a keep alive request
    SwiftRobotPacketTypeKeepAliveResponse = 12,
    /// connection request. Should be sent right after a connection is established. Payload must be a `swiftrobot_packet_connect`
    SwiftRobotPacketTypeConnect = 13,
    /// reply to a connection request. Payload must be a `swiftrobot_packet_connect`
    SwiftRobotPacketTypeConnectAck = 14
} swiftrobot_packet_type_t;

///
/// Since usbmuxd binary is deprecated it is reused as swiftrobot packet type
///
typedef enum swiftrobot_packet_protocol {
    SwiftRobotPacketProtocol = 0,
    USBMuxPacketProtocolPlist = 1,
} swiftrobot_packet_protocol_t;

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
