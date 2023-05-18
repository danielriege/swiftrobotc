#pragma once

#include <string>
#include <chrono>

struct ExternalClient {
    std::string clientID;
    std::chrono::system_clock::time_point lastKeepAliveResponse;
    std::vector<uint16_t> subscriptions;
};
