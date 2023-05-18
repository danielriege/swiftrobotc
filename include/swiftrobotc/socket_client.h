#ifndef channel_hpp
#define channel_hpp

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/uio.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <thread>
#include <optional>
#include <arpa/inet.h>
#include <functional>
#include <mutex>

#include "swiftrobotc/swiftrobot_packet.h"

#define USBMUXD_SOCKET_ADDRESS "/var/run/usbmuxd"

///
/// low level socket handler for sending and receiving swiftrobot packets
///
class SocketClient {
private:
    int socket_fd;
    std::optional<std::thread> listeningThread; // thread on which incoming messages are processed
    char buffer[4]; // This buffer stores the last packet size received
    char *data_buffer; // This buffer stores the rest of the packet besides packet size
    uint32_t data_buffer_capacity; // size of the data_buffer after it has allocated memory
    
    std::mutex send_mutex;
    
public:
    SocketClient();
    
    int open(); // use this for USBMUX
    int open(std::string ip_address, uint16_t port); // use this for WiFi
    int close();
    
    /// sends a buffer wrapped around a swiftrobot_packet/usbmuxd_packet to a socket
    size_t send(swiftrobot_packet_protocol_t protocol, swiftrobot_packet_type_t type, int tag, char* data, size_t size);
    /// sends  2 buffers wrapped around a swiftrobot_packet/usbmuxd_packet to a socket.
    size_t send2(swiftrobot_packet_protocol_t protocol, swiftrobot_packet_type_t type, int tag, char* data1, size_t size1, char* data2, size_t size2);
    /// sends a plist message to socket. This automatically uses usbmuxd protocol
    size_t sendPlist(int tag, char* plist, size_t size); // use when protocol is plist
    /// starts the listening thread
    int startListening(std::function<void(swiftrobot_packet_header_t, char* data, size_t size)> callback);
private:
    void listenLoop(std::function<void(swiftrobot_packet_header_t header, char* data, size_t size)> callback);
    ///
    /// Makes a recv call, but only first 4 bytes to get usbmux message length
    /// @returns message length. Returnes 0 or -1 if error occured
    ///
    size_t recieveMessageSize(char* data, size_t sizeToRecieve = 4);
    ///
    /// Makes a recv call and loads everything besides first 4 bytes into provided buffer.
    /// Makes sure that provided size is recieved, so use 'recieveMessageSize()' to know the size
    ///
    size_t listen(char* data, size_t size);
    uint32_t byteArrayToUInt32(char* data);
    
};

typedef std::shared_ptr<SocketClient> USBMuxClientPtr;

#endif /* swiftrobotc_channel_hpp */
