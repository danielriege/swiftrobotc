#include "swiftrobotc/swiftrobotc.h"
#include "Plist.hpp"

SwiftRobotClient::SwiftRobotClient(uint16_t port): port{port} {
    usbHubPtr = std::make_unique<USBHub>(port);
}

SwiftRobotClient::SwiftRobotClient(std::string ip_address, uint16_t port): port{port} {
    wifiClientPtr = std::make_unique<Device>(ip_address, port);
}

SwiftRobotClient::~SwiftRobotClient() {
    disconnectFromUSBHub();
    disconnectFromWifiServer();
}

void SwiftRobotClient::start() {
    if (usbHubPtr) {
        connectToUSBHub();
    }
    if (wifiClientPtr) {
        connectToWifiServer();
    }
}

void SwiftRobotClient::messageReceived(char *data, size_t size) {
    swiftrobot_packet_header_t* receivedHeader = (swiftrobot_packet_header_t*)data;
    switch (receivedHeader->type) {
        case 0: {
            break;
        }
        // base_msgs
        case UINT8ARRAY_MSG: {notify(receivedHeader->channel, base_msg::UInt8Array::deserialize(data+sizeof(swiftrobot_packet_header_t))); break;}
        case UINT16ARRAY_MSG: {notify(receivedHeader->channel, base_msg::UInt16Array::deserialize(data+sizeof(swiftrobot_packet_header_t))); break;}
        case UINT32ARRAY_MSG: {notify(receivedHeader->channel, base_msg::UInt32Array::deserialize(data+sizeof(swiftrobot_packet_header_t))); break;}
        case INT8ARRAY_MSG: {notify(receivedHeader->channel, base_msg::Int8Array::deserialize(data+sizeof(swiftrobot_packet_header_t))); break;}
        case INT16ARRAY_MSG: {notify(receivedHeader->channel, base_msg::Int16Array::deserialize(data+sizeof(swiftrobot_packet_header_t))); break;}
        case INT32ARRAY_MSG: {notify(receivedHeader->channel, base_msg::Int32Array::deserialize(data+sizeof(swiftrobot_packet_header_t))); break;}
        case FLOATARRAY_MSG: {notify(receivedHeader->channel, base_msg::FloatArray::deserialize(data+sizeof(swiftrobot_packet_header_t))); break;}
        // sensor_msgs
        case IMAGE_MSG: {notify(receivedHeader->channel, sensor_msg::Image::deserialize(data + sizeof(swiftrobot_packet_header_t))); break;}
        case IMU_MSG: {notify(receivedHeader->channel, sensor_msg::IMU::deserialize(data + sizeof(swiftrobot_packet_header_t))); break;}
        // control_msgs
        case DRIVE_MSG: {notify(receivedHeader->channel, control_msg::Drive::deserialize(data + sizeof(swiftrobot_packet_header_t))); break;}
        default: {
            break;
        }
    }
}

void SwiftRobotClient::connectToUSBHub() {
    if (usbHubPtr) {
        usbHubPtr->registerReceiveCallback(std::bind(&SwiftRobotClient::messageReceived, this, std::placeholders::_1, std::placeholders::_2));
        usbHubPtr->registerStatusUpdateCallback([this](uint8_t deviceID, uint8_t status) {
            internal_msg::UpdateMsg msg;
            msg.deviceID = deviceID;
            msg.status = (enum internal_msg::status_t)status;
            notify(0, msg);
        });
        usbHubPtr->startLookingForConnections();
    }
}

void SwiftRobotClient::connectToWifiServer() {
    if (wifiClientPtr) {
        wifiClientPtr->setConnectedCallback([this](uint8_t id, uint8_t connected) {
            if (connected == true) {
                internal_msg::UpdateMsg msg;
                msg.deviceID = 0;
                msg.status = internal_msg::CONNECTED;
                notify(0, msg);
                // TODO: disconnect update message
            }
        });
        wifiClientPtr->startConnection();
        wifiClientPtr->setIncomingCallback(std::bind(&SwiftRobotClient::messageReceived, this, std::placeholders::_1, std::placeholders::_2));
    }
}

void SwiftRobotClient::disconnectFromUSBHub() {
    if (usbHubPtr) {
        usbHubPtr->close();
    }
}

void SwiftRobotClient::disconnectFromWifiServer() {
    if (wifiClientPtr) {
        wifiClientPtr->disconnect();
    }
}
