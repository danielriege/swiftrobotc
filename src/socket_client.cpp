#include "swiftrobotc/socket_client.h"

SocketClient::SocketClient() : send_mutex() {
    socket_fd = 0;
}

///
/// opens usbmuxd socket
///
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
#ifdef __APPLE__
    int on = 1;
    if (setsockopt(socket_fd, SOL_SOCKET, SO_NOSIGPIPE, &on, sizeof(on)) < 0) {
        printf("%s: Error %d setting sockop NOSIGPIPE.\n", __PRETTY_FUNCTION__, errno);
        return -1;
    }
#endif
    
    address.sun_family = AF_UNIX;
    strncpy(address.sun_path, USBMUXD_SOCKET_ADDRESS, sizeof(address.sun_path));
    address.sun_path[sizeof(address.sun_path) - 1] = 0;
    
    if (connect(socket_fd,(struct sockaddr *) &address,sizeof (address)) < 0) {
        printf("%s: Error %d connecting to usbmuxd.\n", __PRETTY_FUNCTION__, errno);
        return -1;
    }
    data_buffer = NULL;
    data_buffer_capacity = 0;
    return 0;
}

///
/// opens TCP socket
///
int SocketClient::open(std::string ip_address, uint16_t port) {
    struct sockaddr_in address;
    
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    if(socket_fd < 0) {
        printf("%s: Error %d opening socket.\n", __PRETTY_FUNCTION__, errno);
        return -1;
    }
    // prevent SIGPIPE
#ifdef __APPLE__
    int on = 1;
    if (setsockopt(socket_fd, SOL_SOCKET, SO_NOSIGPIPE, &on, sizeof(on)) < 0) {
        printf("%s: Error %d setting sockop NOSIGPIPE.\n", __PRETTY_FUNCTION__, errno);
        return -1;
    }
#endif
    
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    // Convert IPv4 and IPv6 addresses from text to binary
    if (::inet_pton(AF_INET, ip_address.c_str(), &address.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
    
    if (connect(socket_fd,(struct sockaddr *) &address,sizeof (address)) < 0) {
        // if connect fails we dont need to print anything since it will just try to reconnect periodically
        return -1;
    }
    data_buffer = NULL;
    data_buffer_capacity = 0;
    return 0;
}

int SocketClient::close() {
    send_mutex.lock();
    if (shutdown(socket_fd, SHUT_RDWR) < 0) {
        send_mutex.unlock();
        return -1;
    }
    send_mutex.unlock();
    if (listeningThread) {
        listeningThread->join();
        listeningThread.reset();
    }
    send_mutex.lock();
    ::close(socket_fd);
    send_mutex.unlock();
    if (data_buffer != NULL) {
        free(data_buffer);
        data_buffer = NULL;
        data_buffer_capacity = 0;
    }
    return 0;
}

size_t SocketClient::send(swiftrobot_packet_protocol_t protocol, swiftrobot_packet_type_t type, int tag, char* data, size_t size) {
    swiftrobot_packet_header_t header;
    header.type = type;
    header.protocol = protocol;
    header.length = (uint32_t)(sizeof(swiftrobot_packet_header_t) + size);
    send_mutex.lock();
    size_t nwritten = 0;
    nwritten += ::send(socket_fd, &header, sizeof(swiftrobot_packet_header_t), 0);
    if (size > 0) {
        nwritten += ::send(socket_fd, data, size, 0);
    }
    send_mutex.unlock();
    return nwritten;
}

size_t SocketClient::send2(swiftrobot_packet_protocol_t protocol, swiftrobot_packet_type_t type, int tag, char* data1, size_t size1, char* data2, size_t size2) {
    swiftrobot_packet_header_t header;
    header.type = type;
    header.protocol = protocol;
    header.length = (uint32_t)(sizeof(swiftrobot_packet_header_t) + size1 + size2);
    // in total we send 3 buffer so create a iovec buffer
    send_mutex.lock();
    size_t nwritten = 0;
    nwritten = nwritten + ::send(socket_fd, &header, sizeof(swiftrobot_packet_header_t), 0);
    if (size1 > 0) {
        nwritten = nwritten + ::send(socket_fd, data1, size1, 0);
    }
    if (size2 > 0) {
        nwritten = nwritten + ::send(socket_fd, data2, size2, 0);
    }
    send_mutex.unlock();
    return nwritten;
}

size_t SocketClient::sendPlist(int tag, char* plist, size_t size) {
    return send(USBMuxPacketProtocolPlist, USBMuxPacketTypePlistPayload, tag, plist, size);
}

size_t SocketClient::recieveMessageSize(char* data, size_t sizeToRecieve) {
    // read only length
    size_t receivedSize = recv(socket_fd, data, sizeToRecieve, 0);
    if (receivedSize <= 0) {
        return receivedSize;
    } else if (receivedSize != 4) {
        return recieveMessageSize(data + receivedSize, 4-receivedSize);
    } else {
        return byteArrayToUInt32(buffer);
    }
}

size_t SocketClient::listen(char* data, size_t size) {
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

void SocketClient::listenLoop(std::function<void(swiftrobot_packet_header_t header, char* data, size_t size)> callback) {
    while (1) {
        swiftrobot_packet_header_t tmp_header;
        size_t data_size = recieveMessageSize(buffer);
        if (data_size < 4) {
            break;
        }
        if (data_size-4 > data_buffer_capacity) {
            if (data_buffer) {
                // delete old array
                free(data_buffer);
                data_buffer = NULL;
                data_buffer_capacity = 0;
            }
            
            data_buffer = (char*)malloc(data_size-4);
            data_buffer_capacity = (uint32_t)data_size-4;
        }
        //char data[data_size-4]; // -4 because we already have the length which is part of a usbmux message
        
        size_t received = listen(data_buffer, data_size-4);
        if (received != data_size-4) { // because listen is recursevly we cant check for <= 0 because the sizes are sumed internaly
            break;
        }
        // load into tmp_header
        tmp_header.length = (uint32_t)data_size;
        tmp_header.protocol = byteArrayToUInt32(data_buffer);
        tmp_header.type = byteArrayToUInt32(data_buffer+4);
        tmp_header.tag = byteArrayToUInt32(data_buffer+8);
        callback(tmp_header, data_buffer+12, received-12);
    }
}

int SocketClient::startListening(std::function<void(swiftrobot_packet_header_t header, char* data, size_t size)> callback) {
    if (listeningThread) {
        listeningThread->join();
    }
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

