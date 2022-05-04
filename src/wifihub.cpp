//#include "swiftrobotc/wifihub.h"
//
//WiFiClient::WiFiClient(std::string ip, uint16_t port) {
//    this->client = SocketClient();
//    this->ip_address = ip;
//    this->port = port;
//    this->receivedPacketCallback = NULL;
//    this->deviceStatusCallback = NULL;
//}
//
//void WiFiClient::connect() {
//   client
//}
//
//void WiFiClient::close() {
//    client.close();
//}
//
//void WiFiClient::createDevice(device_info_t device, uint16_t port) {
//    DevicePtr new_device = std::make_shared<Device>(device, port);
//    devices[device.device_id] = new_device;
//    new_device->startConnection();
//    new_device->setIncomingCallback(receivedPacketCallback);
//    new_device->setConnectedCallback([this](uint8_t id, uint8_t connected) {
//        if (deviceStatusCallback != NULL && connected == true) {
//            deviceStatusCallback(id, DEVICE_STATUS_CONNECTED);
//        }
//    });
//    if (deviceStatusCallback != NULL) {
//        deviceStatusCallback(device.device_id, DEVICE_STATUS_ATTACHED);
//    }
//}
//
//void WiFiClient::removeDevice(int deviceID) {
//    devices.erase(deviceID);
//    if (deviceStatusCallback != NULL) {
//        deviceStatusCallback(deviceID, DEVICE_STATUS_DETACHED);
//    }
//}
//
//void WiFiClient::sendPacket(int deviceID, char* data, size_t size) {
//    devices[deviceID]->send(data, size);
//}
//
//void WiFiClient::sendPacketToAll(char* data, size_t size) {
//    for ( const auto &device_pair : devices) {
//        device_pair.second->send(data, size);
//    }
//}
//
//void WiFiClient::registerReceiveCallback(std::function<void(char* data, size_t size)> callback) {
//    this->receivedPacketCallback = callback;
//}
//
//void WiFiClient::registerStatusUpdateCallback(std::function<void(uint8_t deviceID, uint8_t status)> callback) {
//    this->deviceStatusCallback = callback;
//}
//
//void WiFiClient::broadcastHandler(usbmux_header_t header, char*data, size_t size) {
//    if (header.protocol == USBMuxPacketProtocolPlist) {
//        std::map<std::string, boost::any> plist_message;
//        try {
//            Plist::readPlist(data, size, plist_message);
//        } catch (std::exception& e) {
//            printf("swiftrobotc > USBHub: Error within Plist parsing... dont't know what to do other than discarding\n");
//            return;
//        }
//        std::string type = boost::any_cast<const std::string&>(plist_message.find(USBMUX_KEY_MESSAGETYPE)->second);
//        
//        if (type == USBMUX_MESSAGETYPE_RESULT) {
//            int result_code = (int)boost::any_cast<const int64_t&>(plist_message.find(USBMUX_KEY_NUMBER)->second);
//            handleResult((usbmux_reply_code_t)result_code, header);
//        } else if (type == USBMUX_MESSAGETYPE_ATTACH) {
//            device_info_t deviceInfo = USBHub::parsePropertiesPlistDict(plist_message);
//            createDevice(deviceInfo, this->port);
//        } else if (type == USBMUX_MESSAGETYPE_DETACH) {
//            int device_id = (int)boost::any_cast<const int64_t&>(plist_message.find(USBMUX_KEY_DEVICEID)->second);
//            removeDevice(device_id);
//        }
//    }
//}
//
//void WiFiClient::handleResult(usbmux_reply_code_t reply_code, usbmux_header_t msg_header) {
//    switch (reply_code) {
//        case USBMuxReplyCodeOK:
//            break;
//        case USBMuxReplyCodeBadCommand:
//            break;
//        case USBMuxReplyCodeBadDevice:
//            break;
//        case USBMuxReplyCodeConnectionRefused:
//            break;
//        case USBMuxReplyCodeMalformedRequest:
//            break;
//        case USBMuxReplyCodeBadVersion:
//            break;
//        default:
//            break;
//    }
//}
//
