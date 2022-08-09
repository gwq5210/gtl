/**
 * @file socket_address.h
 * @author gwq5210 (gwq5210@qq.com)
 * @brief 网络地址类，支持ipv4、ipv6、unix domain socket
 * @date 2022-08-08
 *
 * @copyright Copyright (c) 2022. All rights reserved.
 */

#pragma once

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/un.h>

#include <cstdlib>
#include <cstring>
#include <string>

static const int kMaxUDSPathSize = sizeof(sockaddr_un::sun_path);
static const int kMaxAddressSize = sizeof(struct sockaddr_storage);

bool ValidIPv4(const std::string& ipv4_str);
bool ValidIPv6(const std::string& ipv6_str);
bool ValidUDS(const std::string& uds_str);
int ValidPort(const std::string& port_str);
int ValidPort(int port);

// IPv4: ipv4:port, e.g. 127.0.0.1:8080
// IPv6: [ipv6]:port, e.g. [2001:0db8:3c4d:0015:0000:0000:1a2f:1a2b]:8080
// IPv6 地址的大小和格式使得寻址功能大为增强。
// IPv6 地址大小为 128 位。首选 IPv6 地址表示法为 x:x:x:x:x:x:x:x，其中每个 x 是地址的 8 个 16 位部分的十六进制值。IPv6
// 地址范围从 0000:0000:0000:0000:0000:0000:0000:0000 至 ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff。
// 除此首选格式之外，IPv6 地址还可以用其他两种短格式指定：
// 通过省略前导零指定 IPv6 地址。例如，IPv6 地址 1050:0000:0000:0000:0005:0600:300c:326b 可写作
// 1050:0:0:0:5:600:300c:326b。 通过使用双冒号（::）替换一系列零来指定 IPv6 地址。例如，IPv6 地址 ff06:0:0:0:0:0:0:c3
// 可写作 ff06::c3。一个 IP 地址中只可使用一次双冒号。 IPv6 地址的替代格式组合了冒号与点分表示法，因此可将 IPv4
// 地址嵌入到 IPv6 地址中。对最左边 96 个位指定十六进制值，对最右边 32 个位指定十进制值，来指示嵌入的 IPv4
// 地址。在混合的网络环境中工作时，此格式确保 IPv6 节点和 IPv4 节点之间的兼容性。 IPv4 映射的 IPv6
// 地址使用此替代格式。此类型的地址用于将 IPv4 节点表示为 IPv6 地址。它允许 IPv6 应用程序直接与 IPv4
// 应用程序通信。例如，0:0:0:0:0:ffff:192.1.56.10 和 ::ffff:192.1.56.10/96（短格式）
// Unix Domain Socket: unix:/path/to/file, e.g.
// abslute path : unix:/path/to/file.sock
// relative path: unix:path/to/file.sock
class SocketAddress {
 public:
  SocketAddress() { Clear(); }
  explicit SocketAddress(const char* address) { Parse(std::string(address)); }
  explicit SocketAddress(const std::string& address) { Parse(address); }
  SocketAddress(const struct sockaddr* addr, socklen_t socklen) { set_addr(addr, socklen); }
  SocketAddress(const struct sockaddr_storage* addr, socklen_t socklen) { set_addr(addr, socklen); }

  struct sockaddr& addr() {
    return addr_;
  }
  const struct sockaddr& addr() const { return addr_; }
  struct sockaddr_in& addr_in() {
    return addr_in_;
  }
  const struct sockaddr_in& addr_in() const { return addr_in_; }
  struct sockaddr_in6& addr_in6() {
    return addr_in6_;
  }
  const struct sockaddr_in6& addr_in6() const { return addr_in6_; }
  struct sockaddr_un& addr_un() {
    return addr_un_;
  }
  const struct sockaddr_un& addr_un() const { return addr_un_; }
  const struct sockaddr_storage& addr_storage() const { return addr_storage_; }
  sa_family_t family() const { return addr_.sa_family; }
  void set_family(sa_family_t family) { addr_.sa_family = family; }
  int port() const {
    if (family() == AF_INET) {
      return ntohs(addr_in_.sin_port);
    } else if (family() == AF_INET6) {
      return ntohs(addr_in6_.sin6_port);
    }
    return -1;
  }
  bool set_port(const std::string& port_str);
  bool set_port(int port);
  socklen_t& socklen() { return socklen_; }
  socklen_t socklen() const { return socklen_; }
  void set_socklen(socklen_t addrlen) {
    socklen_ = addrlen;
#if defined(__APPLE__)
    addr_.sa_len = addrlen;
#endif
  }
  void set_addr(const struct sockaddr* addr, socklen_t addrlen) {
    memcpy(&addr_, addr, addrlen);
    set_socklen(addrlen);
  }
  void set_addr(const struct sockaddr_storage* addr, socklen_t addrlen) {
    memcpy(&addr_, addr, addrlen);
    set_socklen(addrlen);
  }

  void Clear() {
    socklen_ = kMaxAddressSize;
    memset(&addr_storage_, 0, sizeof(addr_storage_));
  }
  bool Parse(const std::string& address);
  bool ParseIPv4(const std::string& address);
  bool ParseIPv6(const std::string& address);
  bool ParseUDS(const std::string& address);
  std::string ToString() const;

 private:
  socklen_t socklen_ = kMaxAddressSize;  // valid data length of sockaddr
  union {
    struct sockaddr addr_;
    struct sockaddr_in addr_in_;    // IPv4 address
    struct sockaddr_in6 addr_in6_;  // IPv6 address
    struct sockaddr_un addr_un_;    // Unix socket domain address
    struct sockaddr_storage addr_storage_;
  };
};