#pragma once

#include <stdlib.h>
#include <string>
#include <chrono>

#include "swiftrobotc/external_client.h"
#include "swiftrobotc/usbhub.h"
#include "swiftrobotc/msgs.h"
#include "swiftrobotc/dispatch_queue.h"

#define KEEP_ALIVE_WAITING_TIME 1000 // milliseconds
#define KEEP_ALIVE_CHECK_TIMER 300 // milliseconds
#define KEEP_ALIVE_TIMEOUT 2000 // milliseconds
#define KEEP_ALIVE_TIMER_RANDOM_OFFSET_MAX 300 // milliseconds

class ClientDispatcher {
private:
    bool keepAliveCheckRunning = false;
    
    DispatchQueuePtr queue;
    std::string globalServiceName;
    uint16_t port;
    connection_type_t connection_type;
    
    std::shared_ptr<USBHub> usbHubPtr;
    
    std::map<std::string, ExternalClient> clients;
    std::vector<uint16_t> clientChannelSubsciptions;
    
    void startKeepAliveCheckCycle();
    void stopKeepAliveCheckCycle();
    void updateKeepAliveTime(std::string clientid);
    bool isTimedOut(std::string clientid);
    
    void callbackClientDisconnected(std::string clientid);
    void callbackBonjourServiceFound(std::string serviceName, std::string host, uint16_t port);
    void callbackUSBDeviceStatusChanged(int deviceID, uint8_t status);
    
    void multiplexIncomingPacket(std::string clientid, swiftrobot_packet_type_t type, char* data, size_t length);
    void handleMessage(std::string clientid, char* data, size_t length);
    void handleConnect(std::string clientid, char* data, size_t length);
    void handleConnectAck(std::string clientid, char* data, size_t length);
    void handleSubscribeRequest(std::string clientid, char* data, size_t length);
    void handleKeepAliveRequest(std::string clientid);
    void handleKeepAliveResponse(std::string clientid);
    
    void sendMessage(std::string clientid, Message msg, uint16_t channel, uint32_t size, uint16_t type);
    void sendSubscribeRequest(uint16_t channel);
    void sendConnect(std::string clientid);
    void sendConnectForUnkownUSBDevice(int deviceID);
    void sendConnectAck(std::string clientid);
    void sendKeepAliveRequest(std::string clientid);
    void sendKeepAliveResponse(std::string clientid);
    
public:
    std::function<void(uint16_t, message_packet_header_t* header, char* data)> didReceiveMessageCallback;
    std::function<void(std::string cliendID, internal_msg::status_t)> didReceiveUpdateMsg;
    
    ClientDispatcher(std::string globalServiceName, uint16_t port, connection_type_t connection_type, std::shared_ptr<dispatch_queue> queue);
    
    void start();
    void stop();
    
    void dispatchMessage(uint16_t channel, Message msg, uint32_t size, uint16_t type);
    void subscribeRequest(uint16_t channel);
    
};
