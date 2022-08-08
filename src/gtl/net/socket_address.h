#pragma once

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/un.h>

// IPv4: ipv4:port, e.g. 127.0.0.1:80
// IPv6: ipv6:port, e.g. 
class SocketAddress {
 public:
  struct sockaddr& addr() {
    return addr_;
  }
  const struct sockaddr& addr() const { return addr_; }
  sa_family_t family() const { return addr_.sa_family; }

 private:
  union {
    struct sockaddr addr_;
    struct sockaddr_in addr_in_;    // IPv4 address
    struct sockaddr_in6 addr_in6_;  // IPv6 address
    struct sockaddr_un addr_un_;    // Unix socket domain address
    struct sockaddr_storage addr_storage_;
  };
};