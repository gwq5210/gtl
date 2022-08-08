#include "net/socket_address.h"

#include <boost/algorithm/string.hpp>
#include <string>

bool ValidIPv4(const std::string& ipv4_str) {
  return true;
}

bool ValidIPv6(const std::string& ipv6_str) {
  return true;
}

bool ValidUDS(const std::string& uds_str) {
  return true;
}

bool ValidSocketAddress(const std::string& address) {
  return true;
}