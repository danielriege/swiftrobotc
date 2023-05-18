#include "swiftrobotc/swiftrobotc.h"

SwiftRobotClient::SwiftRobotClient(connection_type_t connection_type, std::string name, uint16_t port): port{port}, connection_type{connection_type}, name{name} {
    
    queue = std::make_shared<dispatch_queue>(SUBSCRIBER_DISPATCH_QUEUE_THEARDS);
    client_dispatcher = std::make_unique<ClientDispatcher>(name, port,connection_type, queue);
    
    client_dispatcher->didReceiveMessageCallback = std::bind(&SwiftRobotClient::didReceiveMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    client_dispatcher->didReceiveUpdateMsg = std::bind(&SwiftRobotClient::didReceiveUpdate, this, std::placeholders::_1, std::placeholders::_2);
}

SwiftRobotClient::~SwiftRobotClient() {
    stop();
}

void SwiftRobotClient::start() {
    client_dispatcher->start();
}

void SwiftRobotClient::stop() {
    client_dispatcher->stop();
}

void SwiftRobotClient::didReceiveMessage(uint16_t channel, message_packet_header_t* header, char* data) {
    switch (header->type) {
        case 0: {
            break;
        }
        // base_msgs
        case UINT8ARRAY_MSG: {notify(header->channel, base_msg::UInt8Array::deserialize(data)); break;}
        case UINT16ARRAY_MSG: {notify(header->channel, base_msg::UInt16Array::deserialize(data)); break;}
        case UINT32ARRAY_MSG: {notify(header->channel, base_msg::UInt32Array::deserialize(data)); break;}
        case INT8ARRAY_MSG: {notify(header->channel, base_msg::Int8Array::deserialize(data)); break;}
        case INT16ARRAY_MSG: {notify(header->channel, base_msg::Int16Array::deserialize(data)); break;}
        case INT32ARRAY_MSG: {notify(header->channel, base_msg::Int32Array::deserialize(data)); break;}
        case FLOATARRAY_MSG: {notify(header->channel, base_msg::FloatArray::deserialize(data)); break;}
        // sensor_msgs
        case IMAGE_MSG: {notify(header->channel, sensor_msg::Image::deserialize(data)); break;}
        case IMU_MSG: {notify(header->channel, sensor_msg::IMU::deserialize(data)); break;}
        // control_msgs
        case DRIVE_MSG: {notify(header->channel, control_msg::Drive::deserialize(data)); break;}
        // nav_msg
        case ODOMETRY_MSG: {notify(header->channel, nav_msg::Odometry::deserialize(data));break;}
        default: {
            break;
        }
    }
}

void SwiftRobotClient::didReceiveUpdate(std::string cliendID, internal_msg::status_t status) {
    internal_msg::UpdateMsg msg;
    msg.clientID = cliendID;
    msg.status = status;
    notify(0, msg);
}
