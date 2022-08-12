#include "gtl/net/socket.h"

int main(int argc, char* argv[]) {
  const char* address_str = "[::1]:9999";
  gtl::SocketAddress server_address(address_str);
  gtl::Socket socket = gtl::Socket::ConnectTo(server_address);
  if (socket < 0) {
    return 0;
  }
  std::string out_msg = "Hello, world!";
  ssize_t ret = socket.SendAll(out_msg);
  GTL_INFO("local address: {}, server address: {}, send size: {}", socket.local_address().ToString(), socket.peer_address().ToString(), ret);
  std::string in_msg = socket.RecvAll(out_msg.size());
  GTL_INFO("recv size: {}, in msg: {}", in_msg.size(), in_msg);
  in_msg = socket.RecvAll(out_msg.size());
  GTL_INFO("recv size: {}, in msg: {}", in_msg.size(), in_msg);
  return 0;
}