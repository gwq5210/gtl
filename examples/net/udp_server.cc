#include "gtl/net/socket.h"

int main(int argc, char* argv[]) {
  GTL_SET_LEVEL(gtl::LogLevel::kDebug);
  const char* address_str = "[::]:9999";
  gtl::SocketAddress server_address(address_str);
  gtl::Socket socket = gtl::Socket::ServerStart(server_address, SOCK_DGRAM);
  if (socket < 0) {
    return 0;
  }

  GTL_INFO("local address: {}", socket.GetLocalAddr().ToString());
  GTL_INFO("peer address: {}", socket.GetPeerAddr().ToString());

  static const int kMaxMsgSize = 65536;
  while (true) {
    gtl::SocketAddress peer_address;
    std::string in_msg = socket.RecvFrom(kMaxMsgSize, &peer_address);
    GTL_INFO("in msg: {} from {}, recv size: {}", in_msg, peer_address.ToString(), in_msg.size());
    ssize_t ret = socket.SendTo(in_msg, peer_address);
    GTL_INFO("send msg ret {}", ret);
    ret = socket.SendTo(in_msg, peer_address);
    GTL_INFO("send msg ret {}", ret);
  }
  return 0;
}