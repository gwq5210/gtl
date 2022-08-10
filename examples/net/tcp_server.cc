#include "gtl/net/socket.h"

int main(int argc, char* argv[]) {
  const char* address_str = "[::]:9999";
  gtl::net::SocketAddress server_address(address_str);
  gtl::net::Socket socket = gtl::net::Socket::ServerStart(server_address);
  if (socket < 0) {
    return 0;
  }
  while (true) {
    gtl::net::SocketAddress peer_address;
    gtl::net::Socket client_socket = socket.Accept(&peer_address);
    if (client_socket < 0) {
      continue;
    }
    std::string in_msg = client_socket.RecvAll(13);
    GTL_INFO("in msg: {} from {}, recv size: {}", in_msg, peer_address.ToString(), in_msg.size());
    ssize_t ret = client_socket.SendAll(in_msg);
    GTL_INFO("send msg ret {}", ret);
    ret = client_socket.SendAll(in_msg);
    GTL_INFO("send msg ret {}", ret);
  }
  return 0;
}