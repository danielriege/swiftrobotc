#pragma once

#include <stlib>
#include <string>

class Server {
private:
    uint16_t port;
    std::string serviceName;
    std::vector<std::shared_ptr<Connection>> connections;
    int server_socket_fd;
    
public:
    Server();
    ~Server();
    
    void start();
    void stop();
    
    void connect();
    void disconnect();
    void sendPacket();
    void sendPacketToAll();
}
