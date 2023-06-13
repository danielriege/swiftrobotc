#pragma once

#include <string>
#include <vector>
#include <chrono>

struct ExternalClient {
    std::string clientID;
    std::chrono::steady_clock::time_point lastKeepAliveResponse;
    std::vector<uint16_t> subscriptions;
};
