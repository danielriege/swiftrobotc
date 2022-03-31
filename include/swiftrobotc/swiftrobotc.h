#ifndef _SWIFTROBOTC_H_
#define _SWIFTROBOTC_H_

#include <stdlib.h>
#include <string.h>
#include <iomanip>
#include <any>

#include "swiftrobotc/usbhub.h"
#include "swiftrobotc/msgs.h"

#define MAX_DATA_SIZE MTU-8 // 8 is size of previus types in packet

typedef struct swiftrobot_packet {
    uint16_t channel;
    uint16_t type;
    uint32_t data_size;
    char data[MAX_DATA_SIZE];
} swiftrobot_packet_t;

typedef enum ConnectionType {
    WiFi,
    USB,
} ConnectionType;

///
/// Manages connections to multipleiOS devices. Connections are made automatically. As soon as a connection is established, subscribe events will come in and publish evenets will be sent to device. There is no queue to store messages in case connection is down.
/// The status of connected devices is published on channel 0 with UpdateMsg type.
///
class SwiftRobotClient {
private:
    uint16_t port;
    USBHub usbHub;
    std::map<uint8_t, std::vector<std::any>> subscriber_channel_map; ///< stores subscriber callbacks as any to be free of generic message type
public:
    ///
    /// Creates a new client and starts looking for connected devices
    /// @param connectionType either WiFi or USB
    /// @param port listening port of the process on iOS device
    ///
    SwiftRobotClient(ConnectionType connectionType, uint16_t port);
    ~SwiftRobotClient();
    
    ///
    ///publishes a message. When connection is down, the messages are discared.
    ///Channel 0 is for internal UpdateMsg
    ///@param channel channel on which so send the message. Works like topics
    ///@param msg message
    ///
    template<typename M>
    void publish(uint8_t channel, M msg) {
        (void)static_cast<Message*>((M*)0);
        if (sizeof(msg) > MAX_DATA_SIZE) {
            printf("Message too long for MTU\n");
            return;
        }
        char byteArray[MAX_DATA_SIZE];
        uint32_t payload_size = msg.serialize(byteArray);
        swiftrobot_packet_t packet;
        packet.channel = channel;
        packet.type = M::type;
        packet.data_size = payload_size;
        memcpy(packet.data, byteArray, payload_size);
        
        //messageReceived((char*)&packet, payload_size+8);
        usbHub.sendPacketToAll((char*)&packet, payload_size+8);
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
    void disconnectFromUSBHub();
    
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
