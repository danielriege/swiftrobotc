#include "swiftrobotc/bonjour_browser.h"

BonjourBrowser::BonjourBrowser(std::string ownServiceName) {
    this->ownServiceName = ownServiceName;
    query.name = BONJOUR_SERVICE;
    query.length = strlen(BONJOUR_SERVICE);
    query.type = mdns_record_type_t::MDNS_RECORDTYPE_IGNORE;

}

void BonjourBrowser::start() {
    struct sockaddr_in saddr;
}

void BonjourBrowser::stop() {
    
}

void BonjourBrowser::setServiceFoundCallback(std::function<void(std::string serviceName, std::string ip_address, uint16_t port)> callback) {
    this->serviceFoundCallback = callback;
}
