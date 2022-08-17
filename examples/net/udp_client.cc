#include "gtl/net/socket.h"

int main(int argc, char* argv[]) {
  GTL_SET_LEVEL(gtl::LogLevel::kDebug);
  const char* address_str = "127.0.0.1:9999";
  gtl::SocketAddress server_address(address_str);
  gtl::Socket socket = gtl::Socket::Create(server_address.family(), SOCK_DGRAM);
  if (socket < 0) {
    return 0;
  }
  std::string out_msg = "Hello, world!";
  ssize_t ret = socket.SendTo(out_msg, server_address);
  GTL_INFO("server address: {}, send size: {}", server_address.ToString(), ret);

  gtl::SocketAddress peer_address;
  std::string in_msg = socket.RecvFrom(out_msg.size(), &peer_address);
  GTL_INFO("server address: {}, in msg: {}, recv size: {}", peer_address.ToString(), in_msg, in_msg.size());

  peer_address.Clear();
  in_msg = socket.RecvFrom(out_msg.size(), &peer_address);
  GTL_INFO("server address: {}, in msg: {}, recv size: {}", peer_address.ToString(), in_msg, in_msg.size());

  GTL_INFO("local address: {}", socket.GetLocalAddr().ToString());
  GTL_INFO("peer address: {}", socket.GetPeerAddr().ToString());
  return 0;
}