#include "swiftrobotc/connection.h"

Connection::Connection(usb_device_info_t usb_device_info, uint16_t port, DispatchQueuePtr queue): client{SocketClient()}, queue{queue} {
    this->usb_info = usb_device_info;
    this->port = port;
    this->packetReceivedCallback = NULL;
    this->connectionStatusCallback = NULL;
    this->status = IDLE;
    this->tag = 1;
    this->usbmux = true;
}

Connection::Connection(std::string name, std::string ip_address, uint16_t port, DispatchQueuePtr queue): client{SocketClient()}, name{name}, queue{queue} {
    usb_device_info_t usb_info;
    usb_info.device_id = 0; // when using wifi, only one connection is allowed, therefore deviceID is always 0
    this->usb_info = usb_info;
    this->ip_address = ip_address;
    this->port = port;
    this->packetReceivedCallback = NULL;
    this->connectionStatusCallback = NULL;
    this->status = IDLE;
    this->tag = 1;
    this->usbmux = false;
}

Connection::~Connection() {
    //disconnect();
}

void Connection::startConnection() {
    status = WANTING_CONNECTION;
    connect();
    //this->reconnectThread = std::thread(&Connection::connectThread, this);
}

void Connection::connect() {
    if (status == WANTING_CONNECTION) {
        client.close();
        if (usbmux) {
            client.open();
            client.startListening(std::bind(&Connection::messageReceived, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
            // send TCP tunneling request
            uint16_t port_bigendian = ((port<<8) & 0xFF00) | (port>>8);
            std::vector<char> packet = Connection::createUSBMuxPacket(USBMUX_MESSAGETYPE_CONNECT,
                {{USBMUX_KEY_DEVICEID,(int)usb_info.device_id},
                {USBMUX_KEY_PORT, int(port_bigendian)}
            });
            if (client.sendPlist(tag, (char*)&packet[0], packet.size()) < 0) {
                return;
            }
            tag++;
            this->status = CONNECTING;
            
        } else {
            if (client.open(ip_address, port) == 0) {
                this->status = CONNECTED;
                client.startListening(std::bind(&Connection::messageReceived, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
                connectedRoutine();
            } else {
                this->status = WANTING_CONNECTION;
            }
        }
    }
}

void Connection::connectedRoutine() {
    if (connectionStatusCallback != NULL) {
        connectionStatusCallback(name,true);
    }
}

void Connection::connectThread() {
    while (status != CONNECTED) {
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        if (status != CONNECTED) {
            connect();
        }
    }
}

void Connection::disconnect() {
    if (status != IDLE) {
        status = CONNECTED; // to abort reconnecting thread
//        if (reconnectThread) {
//            this->reconnectThread->join(); // wait for reconnecting thread to close
//        }
        client.close();
        if (connectionStatusCallback != NULL) {
            connectionStatusCallback(name,false);
        }
    }
    status = IDLE;
}

connection_status_t Connection::getStatus() {
    return status;
}

std::string Connection::getName() {
    return name;
}

void Connection::send(swiftrobot_packet_type_t type, char* data, size_t size) {
    if (status == CONNECTED) {
        client.send(SwiftRobotPacketProtocol, type, tag, data, size);
        tag++;
    }
}

void Connection::send2(swiftrobot_packet_type_t type, char* data1, size_t size1, char* data2, size_t size2) {
    if (status == CONNECTED) {
        client.send2(SwiftRobotPacketProtocol, type, tag, data1, size1, data2, size2);
        tag++;
    }
}

void Connection::setPacketReceivedCallback(std::function<void(std::string name, swiftrobot_packet_type_t type, char* data, size_t size)> callback) {
    this->packetReceivedCallback = callback;
}

void Connection::setConnectionStatusCallback(std::function<void(std::string name, uint8_t connected)> callback) {
    this->connectionStatusCallback = callback;
}

void Connection::messageReceived(swiftrobot_packet_header_t header, char* data, size_t size) {
    if (header.protocol == USBMuxPacketProtocolPlist) {
        std::map<std::string, boost::any> plist_message;
        try {
            Plist::readPlist(data, size, plist_message);
        } catch (std::exception& e) {
            printf("swiftrobotc: Error within Plist parsing... dont't know what to do other than discarding\n");
            return;
        }
        std::string type = boost::any_cast<const std::string&>(plist_message.find(USBMUX_KEY_MESSAGETYPE)->second);
        
        if (type == USBMUX_MESSAGETYPE_RESULT) {
            int result_code = (int)boost::any_cast<const int64_t&>(plist_message.find(USBMUX_KEY_NUMBER)->second);
            switch (result_code) {
                case USBMuxReplyCodeOK: {
                    status = CONNECTED;
                    connectedRoutine();
                    break;
                }
                case USBMuxReplyCodeConnectionRefused: {
                    status = WANTING_CONNECTION;
                    break;
                }
                default:
                    break;
            }
        } else {
            printf("swiftrobotc: received a none result message: %s\n", type.c_str());
        }
    } else if (header.protocol == SwiftRobotPacketProtocol) {
        if (header.type == SwiftRobotPacketTypeConnect || header.type == SwiftRobotPacketTypeConnectAck) {
            this->name = std::string(data);
        }
        if (packetReceivedCallback != NULL) {
            packetReceivedCallback(name, (swiftrobot_packet_type_t)header.type, data, size);
        }
    }
}

// MARK: - keep alive handlng

//bool Connection::checkKeepAliveTimeout() {
//    if (lastKeepAliveRequest + std::chrono::milliseconds(KEEPALIVE_TIMEOUT) < std::chrono::system_clock::now()) {
//        return false;
//    }
//    return true;
//}
//
//void Connection::startKeepAliveCheckCycle() {
//    lastKeepAliveRequest = std::chrono::system_clock::now();
//    if (keepAliveCycleCheckThread) {
//        keepAliveCycleCheckThread->join();
//    }
//    keepAliveCycleCheckThread = std::thread(&Connection::checkKeepAliveThread, this);
//}

//void Connection::checkKeepAliveThread() {
//    while (status == CONNECTED) {
//        std::this_thread::sleep_for(std::chrono::milliseconds(KEEPALIVE_CHECK_TIMER));
//        if (status != CONNECTED) {
//            return;
//        }
//        if (!checkKeepAliveTimeout()) {
//            disconnect();
//            startConnection();
//            return;
//        }
//    }
//}

std::vector<char> Connection::createUSBMuxPacket(std::string message_type, std::map<std::string, boost::any> additionalPayload) {
    std::map<std::string, boost::any> packet_dict;
    packet_dict[USBMUX_KEY_CLIENTNAME] = std::string(USBMUX_CLIENTNAME);
    packet_dict[USBMUX_KEY_CLIENTVERSION] = std::string(USBMUX_CLIENTVERSION);
    packet_dict[USBMUX_KEY_MESSAGETYPE] = message_type;
    for ( const auto &packet_piece : additionalPayload) {
        packet_dict[packet_piece.first] = packet_piece.second;
    }
    std::vector<char> packet;
    Plist::writePlistXML(packet, packet_dict);
    return packet;
}

std::vector<char> Connection::createUSBMuxPacket(std::string message_type) {
    return createUSBMuxPacket(message_type, std::map<std::string, boost::any>());
}
