#include "swiftrobotc/client_dispatcher.h"

ClientDispatcher::ClientDispatcher(std::string globalServiceName, uint16_t port, connection_type_t connection_type, DispatchQueuePtr queue): globalServiceName(globalServiceName), port(port), connection_type{connection_type}, queue{queue}  {
    
    if (connection_type == ::USB) {
        usbHubPtr = std::make_shared<USBHub>(port, queue);
        usbHubPtr->registerReceiveCallback(std::bind(&ClientDispatcher::multiplexIncomingPacket, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
        usbHubPtr->registerStatusUpdateCallback(std::bind(&ClientDispatcher::callbackUSBDeviceStatusChanged, this, std::placeholders::_1, std::placeholders::_2));
    }
}

void ClientDispatcher::start() {
    if (connection_type == connection_type_t::USB && usbHubPtr) {
        usbHubPtr->startLookingForConnections();
        startKeepAliveCheckCycle();
    } else if (connection_type == connection_type_t::WIFI) {
        
    }
}

void ClientDispatcher::stop() {
    if (connection_type == connection_type_t::USB && usbHubPtr) {
        usbHubPtr->close();
    }
}

void ClientDispatcher::dispatchMessage(uint16_t channel, Message msg, uint32_t size, uint16_t type) {
    for (auto const& [clientid, client]: clients) {
        for (auto subscription_channel: client.subscriptions) {
            if (subscription_channel == channel) {
                sendMessage(clientid, msg, channel, size, type);
            }
        }
    }
}

void ClientDispatcher::subscribeRequest(uint16_t channel) {
    clientChannelSubsciptions.push_back(channel);
    sendSubscribeRequest(channel);
}

// MARK:  keep alive handling

void ClientDispatcher::startKeepAliveCheckCycle() {
    keepAliveCheckRunning = true;
    
    int randomOffset = std::rand() % KEEP_ALIVE_TIMER_RANDOM_OFFSET_MAX;
    queue->dispatch_after(dispatch_queue::now() + std::chrono::milliseconds(KEEP_ALIVE_CHECK_TIMER + randomOffset), [&] {
        if (!keepAliveCheckRunning) {
            return;
        }
        for (auto const& [clientid, client]: clients) {
            std::string clientid_ = clientid; // to make clang compiler happy
            if (isTimedOut(clientid)) {
                sendKeepAliveRequest(clientid);
                queue->dispatch_after(dispatch_queue::now() + std::chrono::milliseconds(KEEP_ALIVE_WAITING_TIME), [this, clientid_] {
                    if (!keepAliveCheckRunning) {
                        return;
                    }
                    if (isTimedOut(clientid_)) {
                        // disconnect from client
                        if (usbHubPtr) {
                            usbHubPtr->disconnect(clientid_);
                        }
                    }
                });
            }
        }
        startKeepAliveCheckCycle();
    });
}

void ClientDispatcher::stopKeepAliveCheckCycle() {
    keepAliveCheckRunning = false;
}

void ClientDispatcher::updateKeepAliveTime(std::string clientid) {
    clients[clientid].lastKeepAliveResponse = std::chrono::steady_clock::now();
}

bool ClientDispatcher::isTimedOut(std::string clientid) {
    if (clients[clientid].lastKeepAliveResponse + std::chrono::milliseconds(KEEP_ALIVE_TIMEOUT) < std::chrono::steady_clock::now()) {
        return true;
    }
    return false;
}

// MARK: Callbacks

void ClientDispatcher::callbackClientDisconnected(std::string clientid) {
    
}

void ClientDispatcher::callbackBonjourServiceFound(std::string serviceName, std::string host, uint16_t port) {
    
}

void ClientDispatcher::callbackUSBDeviceStatusChanged(int deviceID, uint8_t status) {
    if (status == DEVICE_STATUS_ATTACHED || status == DEVICE_STATUS_DETACHED) {
        didReceiveUpdateMsg("",(internal_msg::status_t)status);
    } else if (status == DEVICE_STATUS_CONNECTED) {
        sendConnectForUnkownUSBDevice(deviceID);
    }
}

// MARK: Incoming


void ClientDispatcher::multiplexIncomingPacket(std::string clientid, swiftrobot_packet_type_t type, char* data, size_t length) {
    switch (type) {
        case USBMuxPacketTypeResult:
        case USBMuxPacketTypeConnect:
        case USBMuxPacketTypeListen:
        case USBMuxPacketTypeDeviceAdd:
        case USBMuxPacketTypeDeviceRemove:
        case USBMuxPacketTypePlistPayload:
            break;
        case ::SwiftRobotPacketTypeMessage:
            handleMessage(clientid, data, length);
            break;
        case SwiftRobotPacketTypeSubscribeRequest:
            handleSubscribeRequest(clientid, data, length);
            break;
        case SwiftRobotPacketTypeKeepAliveRequest:
            handleKeepAliveRequest(clientid);
            break;
        case SwiftRobotPacketTypeKeepAliveResponse:
            handleKeepAliveResponse(clientid);
            break;
        case SwiftRobotPacketTypeConnect:
            handleConnect(clientid, data, length);
            break;
        case SwiftRobotPacketTypeConnectAck:
            handleConnectAck(clientid, data, length);
            break;
    }
}

void ClientDispatcher::handleMessage(std::string clientid, char* data, size_t length) {
    updateKeepAliveTime(clientid);
    
    message_packet_header_t* receivedHeader = (message_packet_header_t*)data;
    didReceiveMessageCallback(receivedHeader->channel, receivedHeader, data+sizeof(message_packet_header_t));
}

void ClientDispatcher::handleConnect(std::string clientid, char* data, size_t length) {
    handleConnectAck(clientid, data, length);
    sendConnectAck(clientid);
}

void ClientDispatcher::handleConnectAck(std::string clientid, char* data, size_t length) {
    ExternalClient new_client{clientid, std::chrono::steady_clock::now()};
    
    message_connect_header_t connect_msg = message_connect_header_t::deserialize(data);
    new_client.subscriptions = connect_msg.channels;
    
    clients[clientid] = new_client;
    didReceiveUpdateMsg(clientid, internal_msg::status_t::CONNECTED);
}

void ClientDispatcher::handleSubscribeRequest(std::string clientid, char* data, size_t length) {
    subscribe_request_packet_header_t* subscribe_request = (subscribe_request_packet_header_t*)data;
    clients[clientid].subscriptions.push_back(subscribe_request->channel);
}

void ClientDispatcher::handleKeepAliveRequest(std::string clientid) {
    updateKeepAliveTime(clientid);
    sendKeepAliveResponse(clientid);
}

void ClientDispatcher::handleKeepAliveResponse(std::string clientid) {
    updateKeepAliveTime(clientid);
}

// MARK: Outgoing

void ClientDispatcher::sendMessage(std::string clientid, Message msg, uint16_t channel, uint32_t size, uint16_t type) {
    char packet[size]; // 8 is size of packet headers
    msg.serialize(packet); // copy data into packet array after header
    
    message_packet_header_t header;
    header.channel = channel;
    header.type = type;
    header.data_size = size;
    
    if (usbHubPtr) {
        usbHubPtr->send2Packet(::SwiftRobotPacketTypeMessage, clientid, (char*)&header, sizeof(header), packet, size);
    }
}

void ClientDispatcher::sendSubscribeRequest(uint16_t channel) {
    subscribe_request_packet_header_t subscribe_request;
    subscribe_request.channel = channel;
    
    if (usbHubPtr) {
        usbHubPtr->sendPacketToAll(::SwiftRobotPacketTypeSubscribeRequest, (char*)&subscribe_request, sizeof(subscribe_request));
    }
}

void ClientDispatcher::sendConnect(std::string clientid) {
    message_connect_header_t connect_msg;
    connect_msg.clientid = globalServiceName;
    connect_msg.subscribers = clientChannelSubsciptions.size();
    connect_msg.channels = clientChannelSubsciptions;
    
    char packet[connect_msg.getSize()];
    connect_msg.serialize(packet);
    
    if (usbHubPtr) {
        usbHubPtr->sendPacket(::SwiftRobotPacketTypeConnect, clientid, packet, connect_msg.getSize());
    }
}

void ClientDispatcher::sendConnectForUnkownUSBDevice(int deviceID) {
    message_connect_header_t connect_msg;
    connect_msg.clientid = globalServiceName;
    connect_msg.subscribers = clientChannelSubsciptions.size();
    connect_msg.channels = clientChannelSubsciptions;
    
    char packet[connect_msg.getSize()];
    connect_msg.serialize(packet);
    
    if (usbHubPtr) {
        usbHubPtr->sendPacket(::SwiftRobotPacketTypeConnect, deviceID, packet, connect_msg.getSize());
    }
}

void ClientDispatcher::sendConnectAck(std::string clientid) {
    message_connect_header_t connect_msg;
    connect_msg.clientid = globalServiceName;
    connect_msg.subscribers = clientChannelSubsciptions.size();
    connect_msg.channels = clientChannelSubsciptions;
    
    char packet[connect_msg.getSize()];
    connect_msg.serialize(packet);
    
    if (usbHubPtr) {
        usbHubPtr->sendPacket(::SwiftRobotPacketTypeConnectAck, clientid, packet, connect_msg.getSize());
    }
}

void ClientDispatcher::sendKeepAliveRequest(std::string clientid) {
    if (usbHubPtr) {
        usbHubPtr->sendPacket(::SwiftRobotPacketTypeKeepAliveRequest, clientid, nullptr, 0);
    }
}

void ClientDispatcher::sendKeepAliveResponse(std::string clientid) {
    if (usbHubPtr) {
        usbHubPtr->sendPacket(::SwiftRobotPacketTypeKeepAliveResponse, clientid, nullptr, 0);
    }
}

