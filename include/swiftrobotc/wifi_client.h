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

typedef struct wifi_packet {
    uint32_t length;
    char* payload;
} wifi_packet_t;

///
/// sends and recieves messages via the usbmux protocol
///
class USBMuxClient {
private:
    int socket_fd;
    std::optional<std::thread> listeningThread;
    char buffer[4];
    
public:
    USBMuxClient();
    
    int open();
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

typedef std::shared_ptr<USBMuxClient> USBMuxClientPtr;

#endif /* swiftrobotc_channel_hpp */
