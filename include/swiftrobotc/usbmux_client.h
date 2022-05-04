#ifndef channel_hpp
#define channel_hpp

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <thread>
#include <optional>
#include <arpa/inet.h>
#include <functional>

#define USBMUXD_SOCKET_ADDRESS "/var/run/usbmuxd"

typedef struct usbmux_header {
    uint32_t length;
    uint32_t protocol;
    uint32_t type;
    uint32_t tag;
} usbmux_header_t;

typedef struct usbmux_packet {
    usbmux_header_t header;
    char* payload;
} usbmux_packet_t;

typedef enum usbmux_packet_type {
    USBMuxPacketTypeResult = 1,
    USBMuxPacketTypeConnect = 2,
    USBMuxPacketTypeListen = 3,
    USBMuxPacketTypeDeviceAdd = 4,
    USBMuxPacketTypeDeviceRemove = 5,
    // ? = 6,
    // ? = 7,
    USBMuxPacketTypePlistPayload = 8, // only supported type by usbmuxd by now
    // Custom Types
    USBMuxPacketTypeApplicationData = 9,
} usbmux_packet_type_t;

typedef enum usbmux_packet_protocol {
    USBMuxPacketProtocolBinary = 0,
    USBMuxPacketProtocolPlist = 1,
} usbmux_packet_protocol_t;

///
/// sends and recieves messages via the usbmux protocol
///
class SocketClient {
private:
    int socket_fd;
    std::optional<std::thread> listeningThread;
    char buffer[4];
    char *data_buffer;
    uint32_t data_buffer_capacity;
    
public:
    SocketClient();
    
    int open(); // use this for USBMUX
    int open(std::string ip_address, uint16_t port); // use this for WiFi
    int close();
    
    int send(usbmux_packet_protocol_t protocol, usbmux_packet_type_t type, int tag, char* data, size_t size);
    int sendPlist(int tag, char* plist, size_t size); // use when protocol is plist
    int startListening(std::function<void(usbmux_header_t, char* data, size_t size)> callback);
private:
    void listenLoop(std::function<void(usbmux_header_t header, char* data, size_t size)> callback);
    ///
    /// Makes a recv call, but only first 4 bytes to get usbmux message length
    /// @returns message length. Returnes 0 or -1 if error occured
    ///
    size_t recieveMessageSize();
    ///
    /// Makes a recv call and loads everything besides first 4 bytes into provided buffer.
    /// Makes sure that provided size is recieved, so use 'recieveMessageSize()' to know the size
    ///
    int listen(char* data, size_t size);
    uint32_t byteArrayToUInt32(char* data);
    
};

typedef std::shared_ptr<SocketClient> USBMuxClientPtr;

#endif /* swiftrobotc_channel_hpp */
