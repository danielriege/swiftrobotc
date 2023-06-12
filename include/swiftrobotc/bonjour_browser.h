#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <functional>

#include "swiftrobotc/mdns.h"

#define BONJOUR_SERVICE "_swiftrobot._tcp.local."

class BonjourBrowser {
private:
    mdns_query_t query;
    int socket_one_shot = 0;
    
    std::string ownServiceName;
    std::function<void(std::string serviceName, std::string ip_address, uint16_t port)> serviceFoundCallback;
public:
    BonjourBrowser(std::string ownServiceName);
    void start();
    void stop();
    void setServiceFoundCallback(std::function<void(std::string serviceName, std::string ip_address, uint16_t port)> callback);
    
};
