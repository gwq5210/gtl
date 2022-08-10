#include "gtl/net/socket.h"

int main(int argc, char* argv[]) {
  const char* address_str = "[::]:9999";
  gtl::net::SocketAddress server_address(address_str);
  gtl::net::Socket socket = gtl::net::Socket::ServerStart(server_address, SOCK_DGRAM);
  if (socket < 0) {
    return 0;
  }

  static const int kMaxMsgSize = 65536;
  while (true) {
    gtl::net::SocketAddress peer_address;
    std::string in_msg = socket.RecvFrom(kMaxMsgSize, &peer_address);
    GTL_INFO("in msg: {} from {}, recv size: {}", in_msg, peer_address.ToString(), in_msg.size());
    ssize_t ret = socket.SendTo(in_msg, peer_address);
    GTL_INFO("send msg ret {}", ret);
    ret = socket.SendTo(in_msg, peer_address);
    GTL_INFO("send msg ret {}", ret);
  }
  return 0;
}