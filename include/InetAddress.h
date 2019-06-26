#pragma once
#include "Common.h"

#include <string>
#include <vector>
#include <netinet/in.h>

typedef struct sockaddr_in sockaddr_in

class InetAddress : copyable {
public:
    // constructor
    InetAddress(StringArg ip, uint16_t port);
    InetAddress(StringArg ipAndPort);
    InetAddress(uint16_t port, bool loopbackOnly = false);
    InetAddress(const struct sockaddr_in &sockAddr):saddr_(sockAddr)
    {}

    std::string toIP() const;
    std::string toIpPort() const;

    // convert ip address to network address
    uint32_t ipNetEndian() const { return saddr_.sin_addr.s_addr; }
    uint16_t portNetEndian() const { return saddr_.sin_port}

    uint32_t ipHostEndian() const { return htonl(saddr_.sin_addr.s_addr); }
    uint16_t portHostEndian() const { return htons(saddr_.sin_port); }

    // get saddr_
    const struct sockaddr_in& getSocketAddress() const { return saddr_; }
    // set saddr_
    void setSocketAddress(const struct sockaddr_in& saddr) { saddr_ = saddr; }

    static bool resolve(StringArg hostname, InetAddress*);
    static std::vector<InetAddress> resolveAll(StringArg hostname, uint16_t port = 0);

    bool operator==(const InetAddress& rhs) const {
        return saddr_.sin_family == rhs.saddr_.sin_family &&
               ipNetEndian() == rhs.ipNetEndian() &&
               portNetEndian() == rhs.portNetEndian();
    }

private:
    static bool resolveSlow(const char* hostname, InetAddress*);
    struct sockaddr_in saddr_;
}