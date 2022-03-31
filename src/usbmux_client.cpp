#include "swiftrobotc/usbmux_client.h"

USBMuxClient::USBMuxClient() {
    socket_fd = 0;
}

int USBMuxClient::open() {
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
    int on = 1;
    if (setsockopt(socket_fd, SOL_SOCKET, SO_NOSIGPIPE, &on, sizeof(on)) < 0) {
        printf("%s: Error %d setting sockop NOSIGPIPE.\n", __PRETTY_FUNCTION__, errno);
        return -1;
    }
    
    address.sun_family = AF_UNIX;
    strncpy(address.sun_path, USBMUXD_SOCKET_ADDRESS, sizeof(address.sun_path));
    address.sun_path[sizeof(address.sun_path) - 1] = 0;
    
    if (connect(socket_fd,(struct sockaddr *) &address,sizeof (address)) < 0) {
        printf("%s: Error %d connecting to usbmuxd.\n", __PRETTY_FUNCTION__, errno);
        return -1;
    }
    return 0;
}

int USBMuxClient::close() {
    if (shutdown(socket_fd, SHUT_RDWR) < 0) {
        return -1;
    }
    if (listeningThread) {
        listeningThread->join();
    }
    ::close(socket_fd);
    return 0;
}

int USBMuxClient::send(usbmux_packet_protocol_t protocol, usbmux_packet_type_t type, int tag, char* data, size_t size) {
    if (size > DATA_MTU) {
        errno = EMSGSIZE;
        return -1;
    }
    usbmux_header_t header;
    header.type = type;
    header.protocol = protocol;
    header.length = sizeof(usbmux_header_t) + size;
    usbmux_packet_t packet;
    packet.header = header;
    memcpy(packet.payload, data, size);
    return ::send(socket_fd, (char*)&packet, header.length, 0);
}

int USBMuxClient::sendPlist(int tag, char* plist, size_t size) {
    return send(USBMuxPacketProtocolPlist, USBMuxPacketTypePlistPayload, tag, plist, size);
}

int USBMuxClient::listen(usbmux_header_t* header, char* data, size_t size) {
    size_t receivedSize = recv(socket_fd, buffer, MTU, 0);
    if (receivedSize <= 0) {
        return receivedSize;
    }
    // at beginning of every message, there is always a header
    usbmux_packet_t* receivedPacket = (usbmux_packet_t*)buffer;
    *header = receivedPacket->header;
    // weird behaviour: sometimes the packet is split into two messages, sometimes not...
    // catch that
    if (receivedSize == sizeof(usbmux_header_t)) {
        // message was split but everything under control, we got the header right
        // just need to receive again
        size_t part2_size = recv(socket_fd, buffer, DATA_MTU, 0);
        if (part2_size == header->length - receivedSize) {
            // we got exactly the remaining part
            memcpy(data, buffer, header->length - receivedSize);
        } else {
            // something wrong, idc anymore
            // use errno from
            return -1;
        }
    } else if (receivedSize == header -> length) {
        // message was not split
        // payload was written into packet, so we can just copy payload from there
        memcpy(data, receivedPacket->payload, receivedSize - sizeof(usbmux_header_t));
    } else {
        // something even weirder happened...
        errno = EMSGSIZE; // message too long, maybe something else suits better
        return -1;
    }
    // sorry for the many if statements
    return header->length - sizeof(usbmux_header_t); // we can guarantee that we received that many bytes for the payload
}

void USBMuxClient::listenLoop(std::function<void(usbmux_header_t header, char* data, size_t size)> callback) {
    while (1) {
        usbmux_header_t tmp_header;
        char data[DATA_MTU];
        size_t received = listen(&tmp_header, data, MTU);
        if (received > 0) {
            callback(tmp_header, data, received);
        } else {
            break;
        }
    }
}

int USBMuxClient::startListening(std::function<void(usbmux_header_t header, char* data, size_t size)> callback) {
    listeningThread = std::thread(&USBMuxClient::listenLoop, this, callback);
}
