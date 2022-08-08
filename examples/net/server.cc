#include "gtl/net/socket.h"

int main(int argc, char* argv[]) {
  const char* address_str = "127.0.0.1:9999";
  SocketAddress server_address(address_str);
  int server_fd = ServerStart(server_address, SOCK_DGRAM);
  if (server_fd < 0) {
    return 0;
  }
  while (true) {
    struct sockaddr_storage addr;
    socklen_t socklen = sizeof(addr);
    SocketAddress peer_address;
    int client_fd = server_fd;
    if (false) {
      client_fd = accept(server_fd, reinterpret_cast<struct sockaddr*>(&addr), &socklen);
      if (client_fd == -1) {
        GTL_ERROR("accept failed: errno:{}, errmsg:{}", errno, strerror(errno));
        continue;
      }
      peer_address.set_addr(&addr, socklen);
      GTL_INFO("new connection from address {}", peer_address.ToString());
    }
    // peer_address.set_family(server_address.family());
    std::string in_msg = RecvMsg(client_fd, peer_address, 13);
    GTL_INFO("in msg: {} from {}", in_msg, peer_address.ToString());
    ssize_t ret = SendMsg(client_fd, peer_address, in_msg);
    GTL_INFO("send msg ret {}", ret);
  }
  return 0;
}