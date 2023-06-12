#ifndef _SWIFTROBOTC_H_
#define _SWIFTROBOTC_H_

#include <stdlib.h>
#include <string.h>
#include <iomanip>
#include <vector>
#include <map>
#include <any>
#include <chrono>
#include <semaphore.h>

#include "swiftrobotc/msgs.h"
#include "swiftrobotc/client_dispatcher.h"
#include "swiftrobotc/dispatch_queue.h"

#define SUBSCRIBER_DISPATCH_QUEUE_THEARDS 6

///
/// Manages connections to multipleiOS devices. Connections are made automatically. As soon as a connection is established, subscribe events will come in and publish evenets will be sent to device. There is no queue to store messages in case connection is down.
/// The status of connected devices is published on channel 0 with UpdateMsg type.
///
class SwiftRobotClient {
    
    struct Subscriber {
        std::any callback;
        std::binary_semaphore semaphore{1};
    };
    
private:
    bool running;
    std::string name;
    uint16_t port;
    connection_type_t connection_type;
    std::map<uint8_t, std::vector<std::shared_ptr<Subscriber>>> subscriber_channel_map;
    std::shared_ptr<dispatch_queue> queue;
    std::unique_ptr<ClientDispatcher> client_dispatcher;
public:
    
    SwiftRobotClient(connection_type_t connection_type, std::string name, uint16_t port = 0);
    ~SwiftRobotClient();
    
    void start();
    void stop();
    
    ///
    ///publishes a message. When connection is down, the messages are discared.
    ///Channel 0 is for internal UpdateMsg
    ///@param channel channel on which so send the message. Works like topics
    ///@param msg message
    ///
    template<typename M>
    void publish(uint8_t channel, M msg) {
        (void)static_cast<Message*>((M*)0);
        // local distribution
        notify(channel, msg);
        
        client_dispatcher->dispatchMessage(channel, msg, msg.getSize(), msg.type);
        
    };
    
    ///
    ///subscribes to a channel by saving data and making a request
    ///Channel 0 is for internal UpdateMsg
    ///@param channel channel on which so send the message. Works like topics
    ///@param callback callback which is called when message of specified type has arrived
    ///
    template<typename M>
    void subscribe(uint8_t channel, std::function<void(M msg)> callback) {
        (void)static_cast<Message*>((M*)0);
        std::shared_ptr<Subscriber> sub = std::make_shared<Subscriber>();
        sub->callback = callback;
        subscriber_channel_map[channel].push_back(sub);
        
        if (channel != 0) {
            client_dispatcher->subscribeRequest(channel);
        }
        
//        if (channel != 0) {
//            subscribe_request_packet_header_t request = {channel};
//            addSubscribeRequest(request);
//        }
    };
private:
    void didReceiveMessage(uint16_t channel, message_packet_header_t* header, char* data);
    void didReceiveUpdate(std::string cliendID, internal_msg::status_t);
    
    ///
    /// internal method to notify all subscribers on that channel with specified message type
    ///
    template<typename M>
    void notify(uint16_t channel,M msg) {
        for (auto subscriber: subscriber_channel_map[channel]) {
            if (subscriber->semaphore.try_acquire()) {
                queue->dispatch([=] {
                    try {
                        auto callback = std::any_cast<std::function<void(M msg)>>(subscriber->callback);
                        callback(msg);
                        subscriber->semaphore.release();
                    } catch (std::bad_any_cast& e) {
                        printf("swiftrobotc: Subscriber on Channel %d has specified wrong message type.\n", channel);
                    }
                });
            }
        }
    }
};

#endif
