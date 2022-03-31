#include "swiftrobotc/swiftrobotc.h"
#include "Plist.hpp"

SwiftRobotClient::SwiftRobotClient(ConnectionType connectionType, uint16_t port): port{port}, usbHub(port) {
    connectToUSBHub();
}

SwiftRobotClient::~SwiftRobotClient() {
    disconnectFromUSBHub();
}

void SwiftRobotClient::messageReceived(char *data, size_t size) {
    swiftrobot_packet_t* receivedPacket = (swiftrobot_packet_t*)data;
    switch (receivedPacket->type) {
        case 0: {
            break;
        }
        // base_msgs
        case UINT8ARRAY_MSG: {notify(receivedPacket->channel, base_msgs::UInt8Array::deserialize(receivedPacket->data)); break;}
        case UINT16ARRAY_MSG: {notify(receivedPacket->channel, base_msgs::UInt16Array::deserialize(receivedPacket->data)); break;}
        case UINT32ARRAY_MSG: {notify(receivedPacket->channel, base_msgs::UInt32Array::deserialize(receivedPacket->data)); break;}
        case INT8ARRAY_MSG: {notify(receivedPacket->channel, base_msgs::Int8Array::deserialize(receivedPacket->data)); break;}
        case INT16ARRAY_MSG: {notify(receivedPacket->channel, base_msgs::Int16Array::deserialize(receivedPacket->data)); break;}
        case INT32ARRAY_MSG: {notify(receivedPacket->channel, base_msgs::Int32Array::deserialize(receivedPacket->data)); break;}
        case FLOATARRAY_MSG: {notify(receivedPacket->channel, base_msgs::FloatArray::deserialize(receivedPacket->data)); break;}
        default: {
            break;
        }
    }
}

void SwiftRobotClient::connectToUSBHub() {
    usbHub.registerReceiveCallback(std::bind(&SwiftRobotClient::messageReceived, this, std::placeholders::_1, std::placeholders::_2));
    usbHub.registerStatusUpdateCallback([this](uint8_t deviceID, uint8_t status) {
        internal_msgs::UpdateMsg msg;
        msg.deviceID = deviceID;
        msg.status = (enum internal_msgs::status_t)status;
        notify(0, msg);
    });
    usbHub.startLookingForConnections();
}

void SwiftRobotClient::disconnectFromUSBHub() {
    usbHub.close();
}
