#include "gtl/net/socket.h"

int main(int argc, char* argv[]) {
  const char* address_str = "[::]:9999";
  gtl::SocketAddress server_address(address_str);
  gtl::Socket socket = gtl::Socket::ServerStart(server_address, SOCK_DGRAM);
  if (socket < 0) {
    return 0;
  }

  gtl::SocketAddress local_address;
  socket.GetLocalAddr(local_address);
  GTL_INFO("local address: {}", local_address.ToString());

  gtl::SocketAddress peer_address;
  socket.GetPeerAddr(peer_address);
  GTL_INFO("peer address: {}", peer_address.ToString());

  static const int kMaxMsgSize = 65536;
  while (true) {
    peer_address.Clear();
    std::string in_msg = socket.RecvFrom(kMaxMsgSize, &peer_address);
    GTL_INFO("in msg: {} from {}, recv size: {}", in_msg, peer_address.ToString(), in_msg.size());
    ssize_t ret = socket.SendTo(in_msg, peer_address);
    GTL_INFO("send msg ret {}", ret);
    ret = socket.SendTo(in_msg, peer_address);
    GTL_INFO("send msg ret {}", ret);
  }
  return 0;
}