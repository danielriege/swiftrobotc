#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <ifaddrs.h>
#include <net/if.h>
#include <netdb.h>
#include <functional>

#include "swiftrobotc/mdns.h"

#define BONJOUR_SERVICE "_swiftrobot._tcp.local."

class BonjourBrowser {
private:
    struct sockaddr_in service_address_ipv4;
    struct sockaddr_in6 service_address_ipv6;
    
    int has_ipv4;
    int has_ipv6;
    
    mdns_query_t query;
    int socket_one_shot = 0;
    
    std::string ownServiceName;
    std::function<void(std::string serviceName, std::string ip_address, uint16_t port)> serviceFoundCallback;
public:
    BonjourBrowser(std::string ownServiceName);
    void doQuery();
    void setServiceFoundCallback(std::function<void(std::string serviceName, std::string ip_address, uint16_t port)> callback);
    
    int open_client_sockets(int* sockets, int max_sockets, int port);
    int query_callback(int sock, const struct sockaddr* from, size_t addrlen, mdns_entry_type_t entry, uint16_t query_id, uint16_t rtype, uint16_t rclass, uint32_t ttl, const void* data, size_t size, size_t name_offset, size_t name_length, size_t record_offset, size_t record_length, void* user_data);
    static mdns_string_t ipv4_address_to_string(char* buffer, size_t capacity, const struct sockaddr_in* addr, size_t addrlen);
    static mdns_string_t ipv6_address_to_string(char* buffer, size_t capacity, const struct sockaddr_in6* addr, size_t addrlen);
};
