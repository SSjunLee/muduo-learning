#pragma once
#include <netinet/in.h>
#include <iostream>
class InetAddress
{
public:
    explicit InetAddress(uint16_t port);
    InetAddress(const std::string &ip, uint16_t port);
    InetAddress(const struct sockaddr_in &addr):addr_(addr){};

    std::string toHostPort() const;

    const struct sockaddr_in& getSocketAddrInet() const {return addr_;}
    void setSocketAddrInet(const struct sockaddr_in& e){addr_ = e;}
private:
    struct sockaddr_in addr_;
};
