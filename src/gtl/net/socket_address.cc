#include "gtl/net/socket_address.h"

#include <cstddef>
#include <string>

#include "gtl/logging.h"

bool ValidIPv4(const std::string& ipv4_str) { return true; }

bool ValidIPv6(const std::string& ipv6_str) { return true; }

bool ValidUDS(const std::string& uds_str) { return uds_str.size() < UDS_PATH_SIZE; }

int ValidPort(const std::string& port_str) {
  if (port_str.size() > 5) {
    return -1;
  }
  for (auto& c : port_str) {
    if (!isdigit(c)) {
      return -1;
    }
  }
  return ValidPort(atoi(port_str.c_str()));
}

int ValidPort(int port) {
  if (port >= 0 && port < 65536) {
    return port;
  }
  return -1;
}

bool ValidSocketAddress(const std::string& address) { return true; }

bool SocketAddress::set_port(const std::string& port_str) {
  int port = ValidPort(port_str);
  if (port < 0) {
    return false;
  }
  return set_port(port);
}

bool SocketAddress::set_port(int port) {
  port = ValidPort(port);
  if (port < 0) {
    return false;
  }
  if (family() == AF_INET) {
    addr_in_.sin_port = htons(port);
  } else if (family() == AF_INET6) {
    addr_in6_.sin6_port = htons(port);
  } else {
    return false;
  }
  return true;
}

bool SocketAddress::Parse(const std::string& address) {
  Clear();
  if (address.find("unix:") == 0) {
    return ParseUDS(address);
  } else if (!address.empty() && address.front() == '[') {
    return ParseIPv6(address);
  } else {
    return ParseIPv4(address);
  }
}

bool SocketAddress::ParseIPv4(const std::string& address) {
  set_family(AF_INET);
  set_socklen(sizeof(struct sockaddr_in));
  std::size_t colon_pos = address.find_last_of(":");
  std::string port_str = address.substr(colon_pos + 1);
  if (!set_port(port_str)) {
    return false;
  }
  std::string ip_str = address.substr(0, colon_pos);
  if (inet_pton(AF_INET, ip_str.c_str(), &addr_in_.sin_addr) <= 0) {
    return false;
  }
  return true;
}

bool SocketAddress::ParseIPv6(const std::string& address) {
  set_family(AF_INET6);
  set_socklen(sizeof(struct sockaddr_in6));
  std::size_t colon_pos = address.find_last_of(":");
  std::string port_str = address.substr(colon_pos + 1);
  if (!set_port(port_str)) {
    return false;
  }
  std::string ip_str = address.substr(0, colon_pos);
  if (ip_str.back() != ']') {
    return false;
  }
  ip_str = ip_str.substr(1, ip_str.size() - 2);
  if (inet_pton(AF_INET6, ip_str.c_str(), &addr_in6_.sin6_addr) <= 0) {
    return false;
  }
  return true;
}

bool SocketAddress::ParseUDS(const std::string& address) {
  set_family(AF_UNIX);
  std::string uds_path = address.substr(5);  // unix:
  if (!ValidUDS(uds_path)) {
    return false;
  }
  memcpy(addr_un_.sun_path, uds_path.c_str(), uds_path.size());
  addr_un_.sun_path[uds_path.size()] = '\0';
  set_socklen(offsetof(struct sockaddr_un, sun_path) + uds_path.size() + 1);
  return true;
}

std::string SocketAddress::ToString() const {
  std::string address;
  if (family() == AF_UNIX) {
    address = std::string("unix:") + addr_un_.sun_path;
  } else if (family() == AF_INET) {
    char ipv4_buf[INET_ADDRSTRLEN];
    if (inet_ntop(AF_INET, &addr_in_.sin_addr, ipv4_buf, INET_ADDRSTRLEN) == NULL) {
      GTL_ERROR("inet_ntop failed, errno:{}, errmsg:{}", errno, strerror(errno));
      return "";
    }
    address = std::string(ipv4_buf) + ":" + std::to_string(port());
  } else if (family() == AF_INET6) {
    char ipv6_buf[INET6_ADDRSTRLEN];
    if (inet_ntop(AF_INET6, &addr_in6_.sin6_addr, ipv6_buf, INET6_ADDRSTRLEN)) {
      GTL_ERROR("inet_ntop failed, errno:{}, errmsg:{}", errno, strerror(errno));
      return "";
    }
    address = "[" + std::string(ipv6_buf) + "]:" + std::to_string(port());
  } else {
    GTL_ERROR("unsupported address family: {}", family());
  }
  return address;
}