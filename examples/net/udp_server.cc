#include "gtl/net/socket.h"

int main(int argc, char* argv[]) {
  const char* address_str = "127.0.0.1:9999";
  SocketAddress server_address(address_str);
  int server_fd = ServerStart(server_address, SOCK_DGRAM);
  if (server_fd < 0) {
    return 0;
  }
  while (true) {
    int client_fd = server_fd;
    SocketAddress peer_address;
    std::string in_msg = RecvMsg(client_fd, peer_address, 65536);
    GTL_INFO("in msg: {} from {}, recv size: {}", in_msg, peer_address.ToString(), in_msg.size());
    ssize_t ret = SendMsg(client_fd, peer_address, in_msg);
    GTL_INFO("send msg ret {}", ret);
  }
  return 0;
}