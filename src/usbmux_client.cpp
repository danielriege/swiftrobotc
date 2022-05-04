#include "swiftrobotc/usbmux_client.h"

SocketClient::SocketClient() {
    socket_fd = 0;
    data_buffer = NULL;
    data_buffer_capacity = 0;
}

int SocketClient::open() {
    struct stat fst;
    struct sockaddr_un address;
    
    // check if socket file exists...
    if (stat(USBMUXD_SOCKET_ADDRESS, &fst) != 0) {
        fprintf(stderr, "%s: File '%s' does not exist! Make sure usbmuxd is installed!\n", __PRETTY_FUNCTION__, USBMUXD_SOCKET_ADDRESS);
        return -1;
    }
    // ... and if it is a unix domain socket
    if (!S_ISSOCK(fst.st_mode)) {
        fprintf(stderr, "%s: File '%s' is not a socket!\n", __PRETTY_FUNCTION__, USBMUXD_SOCKET_ADDRESS);
        return -1;
    }
    socket_fd = socket(PF_UNIX, SOCK_STREAM, 0);

    if(socket_fd < 0) {
        printf("%s: Error %d opening socket.\n", __PRETTY_FUNCTION__, errno);
        return -1;
    }
    
    // prevent SIGPIPE
//    int on = 1;
//    if (setsockopt(socket_fd, SOL_SOCKET, SO_NOSIGPIPE, &on, sizeof(on)) < 0) {
//        printf("%s: Error %d setting sockop NOSIGPIPE.\n", __PRETTY_FUNCTION__, errno);
//        return -1;
//    }
// does not exist on Linux   
    address.sun_family = AF_UNIX;
    strncpy(address.sun_path, USBMUXD_SOCKET_ADDRESS, sizeof(address.sun_path));
    address.sun_path[sizeof(address.sun_path) - 1] = 0;
    
    if (connect(socket_fd,(struct sockaddr *) &address,sizeof (address)) < 0) {
        printf("%s: Error %d connecting to usbmuxd.\n", __PRETTY_FUNCTION__, errno);
        return -1;
    }
    return 0;
}

int SocketClient::open(std::string ip_address, uint16_t port) {
    struct sockaddr_in address;
    
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    if(socket_fd < 0) {
        printf("%s: Error %d opening socket.\n", __PRETTY_FUNCTION__, errno);
        return -1;
    }
    // prevent SIGPIPE
//    int on = 1;
//    if (setsockopt(socket_fd, SOL_SOCKET, SO_NOSIGPIPE, &on, sizeof(on)) < 0) {
//        printf("%s: Error %d setting sockop NOSIGPIPE.\n", __PRETTY_FUNCTION__, errno);
//        return -1;
//    }
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    // Convert IPv4 and IPv6 addresses from text to binary
    // form
    if (::inet_pton(AF_INET, ip_address.c_str(), &address.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
    
    if (connect(socket_fd,(struct sockaddr *) &address,sizeof (address)) < 0) {
        printf("%s: Error %d connecting to server.\n", __PRETTY_FUNCTION__, errno);
        return -1;
    }
    return 0;
}

int SocketClient::close() {
    if (shutdown(socket_fd, SHUT_RDWR) < 0) {
        return -1;
    }
    if (listeningThread) {
        listeningThread->join();
    }
    ::close(socket_fd);
    if (data_buffer != NULL) {
        free(data_buffer);
    }
    return 0;
}

int SocketClient::send(usbmux_packet_protocol_t protocol, usbmux_packet_type_t type, int tag, char* data, size_t size) {
//    if (size > DATA_MTU) {
//        errno = EMSGSIZE;
//        return -1;
//    }
    usbmux_header_t header;
    header.type = type;
    header.protocol = protocol;
    header.length = sizeof(usbmux_header_t) + size;
    usbmux_packet_t packet;
    packet.header = header;
    //memcpy(packet.payload, data, size);
    packet.payload = data;
    char sendBuffer[header.length];
    memcpy(sendBuffer, (char*)&packet, sizeof(usbmux_header_t));
    memcpy(&sendBuffer[sizeof(usbmux_header_t)], packet.payload, size);
    return ::send(socket_fd, sendBuffer, header.length, 0);
}

int SocketClient::sendPlist(int tag, char* plist, size_t size) {
    return send(USBMuxPacketProtocolPlist, USBMuxPacketTypePlistPayload, tag, plist, size);
}

size_t SocketClient::recieveMessageSize() {
    // read only length
    size_t receivedSize = recv(socket_fd, buffer, 4, 0);
    if (receivedSize <= 0) {
        return receivedSize;
    } else if (receivedSize != 4) {
        return -1;
    } else {
        return byteArrayToUInt32(buffer);
    }
}

int SocketClient::listen(char* data, size_t size) {
    // recursiv method to make sure everything is received
    size_t receivedSize = recv(socket_fd, data, size, 0);
    if (receivedSize <= 0) {
        return receivedSize;
    } else if (receivedSize == size) {
        // we have everything we need. return received size
        return receivedSize;
    } else { // we dont need to check if recievedsize > size, because recv does not allow it
        // recieved size is smaller than size, so there has to be more data on socket (we assume)
        // so call ourself recursively
        return receivedSize + listen(data + receivedSize, size-receivedSize);
    }
}


//int USBMuxClient::listen(usbmux_header_t* header, char* data, size_t size) {
//    size_t receivedSize = recv(socket_fd, buffer, MTU, 0);
//    if (receivedSize <= 0) {
//        return receivedSize;
//    }
//    // at beginning of every message, there is always a header
//    usbmux_packet_t* receivedPacket = (usbmux_packet_t*)buffer;
//    *header = receivedPacket->header;
//    // weird behaviour: sometimes the packet is split into two messages, sometimes not...
//    // catch that
//    if (receivedSize == sizeof(usbmux_header_t)) {
//        // message was split but everything under control, we got the header right
//        // just need to receive again
//        size_t part2_size = recv(socket_fd, buffer, DATA_MTU, 0);
//        if (part2_size == header->length - receivedSize) {
//            // we got exactly the remaining part
//            memcpy(data, buffer, header->length - receivedSize);
//        } else {
//            // something wrong, idc anymore
//            // use errno from
//            return -1;
//        }
//    } else if (receivedSize == header -> length) {
//        // message was not split
//        // payload was written into packet, so we can just copy payload from there
//        memcpy(data, receivedPacket->payload, receivedSize - sizeof(usbmux_header_t));
//    } else {
//        // something even weirder happened...
//        errno = EMSGSIZE; // message too long, maybe something else suits better
//        return -1;
//    }
//    // sorry for the many if statements
//    return header->length - sizeof(usbmux_header_t); // we can guarantee that we received that many bytes for the payload
//}

void SocketClient::listenLoop(std::function<void(usbmux_header_t header, char* data, size_t size)> callback) {
    while (1) {
        usbmux_header_t tmp_header;
        size_t data_size = recieveMessageSize();
        if (data_size < 4) {
            break;
        }
        if (data_size-4 > data_buffer_capacity) {
            if (data_buffer != NULL) {
                // delete old array
                free(data_buffer);
                data_buffer = NULL;
            }
            
            data_buffer = (char*)malloc(data_size-4);
            data_buffer_capacity = data_size-4;
        }
        //char data[data_size-4]; // -4 because we already have the length which is part of a usbmux message
        
        size_t received = listen(data_buffer, data_size-4);
        if (received != data_size-4) { // because listen is recursevly we cant check for <= 0 because the sizes are sumed internaly
            break;
        }
        // load into tmp_header
        tmp_header.length = data_size;
        tmp_header.protocol = byteArrayToUInt32(data_buffer);
        tmp_header.type = byteArrayToUInt32(data_buffer+4);
        tmp_header.tag = byteArrayToUInt32(data_buffer+8);
        callback(tmp_header, data_buffer+12, received-12);
    }
}

int SocketClient::startListening(std::function<void(usbmux_header_t header, char* data, size_t size)> callback) {
    listeningThread = std::thread(&SocketClient::listenLoop, this, callback);
}

uint32_t SocketClient::byteArrayToUInt32(char* data) {
    uint32_t value = 0;
    value |= (uint8_t)data[0];
    value |= (uint8_t)data[1] << 8;
    value |= (uint8_t)data[2] << 16;
    value |= (uint8_t)data[3] << 24;
    return value;
}

