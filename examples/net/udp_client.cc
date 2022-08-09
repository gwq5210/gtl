#include "gtl/net/socket.h"

int main(int argc, char* argv[]) {
  const char* address_str = "127.0.0.1:9999";
  SocketAddress server_address(address_str);
  int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  std::string out_msg = "Hello, world!";
  ssize_t ret = SendMsg(sockfd, server_address, out_msg);
  GTL_INFO("server address: {}, send size: {}", server_address.ToString(), ret);
  std::string in_msg = RecvMsg(sockfd, server_address, out_msg.size());
  GTL_INFO("server address: {}, in msg: {}, recv size: {}", server_address.ToString(), in_msg, in_msg.size());
  return 0;
}