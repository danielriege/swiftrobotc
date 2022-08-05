#ifndef _SWIFTROBOTC_H_
#define _SWIFTROBOTC_H_

#include <stdlib.h>
#include <string.h>
#include <iomanip>
#include <any>

#include "swiftrobotc/usbhub.h"
#include "swiftrobotc/msgs.h"

typedef struct swiftrobot_packet_header {
    uint16_t channel;
    uint16_t type;
    uint32_t data_size;
} swiftrobot_packet_header_t;

///
/// Manages connections to multipleiOS devices. Connections are made automatically. As soon as a connection is established, subscribe events will come in and publish evenets will be sent to device. There is no queue to store messages in case connection is down.
/// The status of connected devices is published on channel 0 with UpdateMsg type.
///
class SwiftRobotClient {
private:
    uint16_t port;
    std::shared_ptr<USBHub> usbHubPtr;
    std::shared_ptr<Device> wifiClientPtr;
    std::map<uint8_t, std::vector<std::any>> subscriber_channel_map; ///< stores subscriber callbacks as any to be free of generic message type
public:
    ///
    /// Creates a new client and starts looking for connected devices
    /// @param connectionType either WiFi or USB
    /// @param port listening port of the process on iOS device
    ///
    SwiftRobotClient(uint16_t port); // USBMUX
    SwiftRobotClient(std::string ip_address, uint16_t port); // WIFI
    ~SwiftRobotClient();
    
    void start();
    
    ///
    ///publishes a message. When connection is down, the messages are discared.
    ///Channel 0 is for internal UpdateMsg
    ///@param channel channel on which so send the message. Works like topics
    ///@param msg message
    ///
    template<typename M>
    void publish(uint8_t channel, M msg) {
        (void)static_cast<Message*>((M*)0);
//        if (msg.getSize() > MAX_DATA_SIZE) { // TODO: sizeof does not work
//            printf("Message too long for MTU\n");
//            return;
//        }
        char packet[msg.getSize()+sizeof(swiftrobot_packet_header_t)]; // 8 is size of packet headers
        uint32_t payload_size = msg.serialize(packet+sizeof(swiftrobot_packet_header_t)); // copy data into packet array after header
        swiftrobot_packet_header_t header;
        header.channel = channel;
        header.type = M::type;
        header.data_size = payload_size;
        memcpy(packet, (char*)&header, sizeof(swiftrobot_packet_header_t));
        //memcpy(packet.data, byteArray, payload_size);
        
        //messageReceived((char*)&packet, payload_size+8);
        if (usbHubPtr) {
            usbHubPtr.get()->sendPacketToAll(packet, payload_size+sizeof(swiftrobot_packet_header_t));
        }
        if (wifiClientPtr) {
            wifiClientPtr.get()->send(packet, payload_size+sizeof(swiftrobot_packet_header_t));
        }
    };
    
    ///
    ///subscribes to a channel.
    ///Channel 0 is for internal UpdateMsg
    ///@param channel channel on which so send the message. Works like topics
    ///@param callback callback which is called when message of specified type has arrived
    ///
    template<typename M>
    void subscribe(uint8_t channel, std::function<void(M msg)> callback) {
        (void)static_cast<Message*>((M*)0);
        subscriber_channel_map[channel].push_back(callback);
    };
private:
    ///
    /// configures USBHub instance by setting recieve callback and starts looking for connections
    ///
    void connectToUSBHub();
    void connectToWifiServer();
    void disconnectFromUSBHub();
    void disconnectFromWifiServer();
    
    void messageReceived(char* data, size_t size);
    
    ///
    /// internal method to notify all subscribers on that channel with specified message type
    ///
    template<typename M>
    void notify(uint16_t channel,M msg) {
        for (auto callback_any: subscriber_channel_map[channel]) {
            try {
                auto callback = std::any_cast<std::function<void(M msg)>>(callback_any);
                callback(msg);
            } catch (std::bad_any_cast& e) {
                printf("swiftrobotc: Subscriber on Channel %d has specified wrong message type.\n", channel);
            }
        }
    }
};

#endif
