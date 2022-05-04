#include "swiftrobotc/usbhub.h"

USBHub::USBHub(uint16_t port) {
    this->broadcastClient = SocketClient();
    this->port = port;
    this->receivedPacketCallback = NULL;
    this->deviceStatusCallback = NULL;
}

void USBHub::startLookingForConnections() {
    broadcastClient.open();
    broadcastClient.startListening(std::bind(&USBHub::broadcastHandler, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    
    std::vector<char> packet = Device::createUSBMuxPacket(USBMUX_MESSAGETYPE_LISTEN);
    if (broadcastClient.sendPlist(USBMUX_TAG_BROADCAST, (char*)&packet[0], packet.size()) < 0) {
        return;
    }
}

void USBHub::close() {
    for ( const auto &device_pair : devices) {
        device_pair.second->disconnect();
    }
    broadcastClient.close();
}

void USBHub::createDevice(usb_device_info_t device, uint16_t port) {
    DevicePtr new_device = std::make_shared<Device>(device, port);
    devices[device.device_id] = new_device;
    new_device->startConnection();
    new_device->setIncomingCallback(receivedPacketCallback);
    new_device->setConnectedCallback([this](uint8_t id, uint8_t connected) {
        if (deviceStatusCallback != NULL && connected == true) {
            deviceStatusCallback(id, DEVICE_STATUS_CONNECTED);
        }
    });
    if (deviceStatusCallback != NULL) {
        deviceStatusCallback(device.device_id, DEVICE_STATUS_ATTACHED);
    }
}

void USBHub::removeDevice(int deviceID) {
    devices.erase(deviceID);
    if (deviceStatusCallback != NULL) {
        deviceStatusCallback(deviceID, DEVICE_STATUS_DETACHED);
    }
}

void USBHub::sendPacket(int deviceID, char* data, size_t size) {
    devices[deviceID]->send(data, size);
}

void USBHub::sendPacketToAll(char* data, size_t size) {
    for ( const auto &device_pair : devices) {
        device_pair.second->send(data, size);
    }
}

void USBHub::registerReceiveCallback(std::function<void(char* data, size_t size)> callback) {
    this->receivedPacketCallback = callback;
}

void USBHub::registerStatusUpdateCallback(std::function<void(uint8_t deviceID, uint8_t status)> callback) {
    this->deviceStatusCallback = callback;
}

void USBHub::broadcastHandler(usbmux_header_t header, char*data, size_t size) {
    if (header.protocol == USBMuxPacketProtocolPlist) {
        std::map<std::string, boost::any> plist_message;
        try {
            Plist::readPlist(data, size, plist_message);
        } catch (std::exception& e) {
            printf("swiftrobotc > USBHub: Error within Plist parsing... dont't know what to do other than discarding\n");
            return;
        }
        std::string type = boost::any_cast<const std::string&>(plist_message.find(USBMUX_KEY_MESSAGETYPE)->second);
        
        if (type == USBMUX_MESSAGETYPE_RESULT) {
            int result_code = (int)boost::any_cast<const int64_t&>(plist_message.find(USBMUX_KEY_NUMBER)->second);
            handleResult((usbmux_reply_code_t)result_code, header);
        } else if (type == USBMUX_MESSAGETYPE_ATTACH) {
            usb_device_info_t deviceInfo = USBHub::parsePropertiesPlistDict(plist_message);
            createDevice(deviceInfo, this->port);
        } else if (type == USBMUX_MESSAGETYPE_DETACH) {
            int device_id = (int)boost::any_cast<const int64_t&>(plist_message.find(USBMUX_KEY_DEVICEID)->second);
            removeDevice(device_id);
        }
    }
}

void USBHub::handleResult(usbmux_reply_code_t reply_code, usbmux_header_t msg_header) {
    switch (reply_code) {
        case USBMuxReplyCodeOK:
            break;
        case USBMuxReplyCodeBadCommand:
            break;
        case USBMuxReplyCodeBadDevice:
            break;
        case USBMuxReplyCodeConnectionRefused:
            break;
        case USBMuxReplyCodeMalformedRequest:
            break;
        case USBMuxReplyCodeBadVersion:
            break;
        default:
            break;
    }
}

usb_device_info_t USBHub::parsePropertiesPlistDict(std::map<std::string, boost::any> plist) {
    const std::map<std::string, boost::any>& dict = boost::any_cast<const std::map<std::string, boost::any>&>(plist.find("Properties")->second);
    usb_device_info_t device_info;
    device_info.device_id = (uint32_t)boost::any_cast<const int64_t&>(dict.find("DeviceID")->second);
    device_info.location_id = (uint32_t)boost::any_cast<const int64_t&>(dict.find("LocationID")->second);
    device_info.product_id = (uint32_t)boost::any_cast<const int64_t&>(dict.find("ProductID")->second);
    device_info.connection_speed = (uint64_t)boost::any_cast<const int64_t&>(dict.find("ConnectionSpeed")->second);
    device_info.connection_type = boost::any_cast<const std::string&>(dict.find("ConnectionType")->second);
    device_info.serial_number = boost::any_cast<const std::string&>(dict.find("SerialNumber")->second);
    device_info.udid = boost::any_cast<const std::string&>(dict.find("UDID")->second);
    device_info.usb_serial_number = boost::any_cast<const std::string&>(dict.find("USBSerialNumber")->second);
    return device_info;
}
