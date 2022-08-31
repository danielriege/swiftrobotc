#include "swiftrobotc/device.h"

Device::Device(usb_device_info_t usb_device_info, uint16_t port) {
    this->client = SocketClient();
    this->usb_info = usb_device_info;
    this->port = port;
    this->outgoingMessage = NULL;
    this->connectedCallback = NULL;
    this->status = IDLE;
    this->tag = 1;
    this->reconnectThread = std::thread(&Device::connectThread, this);
    this->usbmux = true;
}

Device::Device(std::string ip_address, uint16_t port) {
    this->client = SocketClient();
    usb_device_info_t usb_info;
    usb_info.device_id = 0; // when using wifi, only one connection is allowed, therefore deviceID is always 0
    this->usb_info = usb_info;
    this->ip_address = ip_address;
    this->port = port;
    this->outgoingMessage = NULL;
    this->connectedCallback = NULL;
    this->status = IDLE;
    this->tag = 1;
    this->reconnectThread = std::thread(&Device::connectThread, this);
    this->usbmux = false;
}

Device::~Device() {
    //disconnect();
}

void Device::startConnection() {
    status = WANTING_CONNECTION;
    connect();
}

void Device::connect() {
    if (status == WANTING_CONNECTION) {
        client.close();
        if (usbmux) {
            client.open();
            client.startListening(std::bind(&Device::messageReceived, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
            // send TCP tunneling request
            uint16_t port_bigendian = ((port<<8) & 0xFF00) | (port>>8);
            std::vector<char> packet = Device::createUSBMuxPacket(USBMUX_MESSAGETYPE_CONNECT,
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
                if (connectedCallback != NULL) {
                    connectedCallback(usb_info.device_id,true);
                }
            } else {
                this->status = WANTING_CONNECTION;
            }
        }
    }
}

void Device::connectThread() {
    while (status != CONNECTED) {
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        connect();
    }
}

void Device::disconnect() {
    if (status != IDLE) {
        status = CONNECTED; // to abort reconnecting thread
        this->reconnectThread.join(); // wait for reconnecting thread to close
        client.close();
    }
    status = IDLE;
}

device_status_t Device::getStatus() {
    return status;
}

void Device::send(char* data, size_t size) {
    // throw exception when send is not successfull
    // TODO: put in queue if not connected
    if (status == CONNECTED) {
        client.send(USBMuxPacketProtocolBinary, USBMuxPacketTypeApplicationData, tag, data, size);
        tag++;
    }
}

void Device::setIncomingCallback(std::function<void(char* data, size_t size)> callback) {
    this->outgoingMessage = callback;
}

void Device::setConnectedCallback(std::function<void(uint8_t deviceID, uint8_t connected)> callback) {
    this->connectedCallback = callback;
}

void Device::messageReceived(usbmux_header_t header, char* data, size_t size) {
    if (header.protocol == USBMuxPacketProtocolPlist) {
        std::map<std::string, boost::any> plist_message;
        try {
            Plist::readPlist(data, size, plist_message);
        } catch (std::exception& e) {
            printf("Device: Error within Plist parsing... dont't know what to do other than discarding\n");
            return;
        }
        std::string type = boost::any_cast<const std::string&>(plist_message.find(USBMUX_KEY_MESSAGETYPE)->second);
        
        if (type == USBMUX_MESSAGETYPE_RESULT) {
            int result_code = (int)boost::any_cast<const int64_t&>(plist_message.find(USBMUX_KEY_NUMBER)->second);
            switch (result_code) {
                case USBMuxReplyCodeOK: {
                    status = CONNECTED;
                    if (connectedCallback != NULL) {
                        connectedCallback(usb_info.device_id,true);
                    }
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
            printf("received a none result message: %s\n", type.c_str());
        }
    } else if (header.protocol == USBMuxPacketProtocolBinary) {
        if (outgoingMessage != NULL) {
            outgoingMessage(data, size);
        }
    }
}

std::vector<char> Device::createUSBMuxPacket(std::string message_type, std::map<std::string, boost::any> additionalPayload) {
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

std::vector<char> Device::createUSBMuxPacket(std::string message_type) {
    return createUSBMuxPacket(message_type, std::map<std::string, boost::any>());
}
