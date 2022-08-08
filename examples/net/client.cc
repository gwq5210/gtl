#include "gtl/net/socket.h"

int main(int argc, char* argv[]) {
  const char* address_str = "127.0.0.1:9999";
  SocketAddress server_address(address_str);
  // Socket socket;
  // if (!socket.Connect(server_address, SOCK_DGRAM)) {
  //   return 0;
  // }
  int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  std::string out_msg = "Hello, world!";
  // socket.SendMsg(out_msg);
  // std::string in_msg = socket.RecvMsg(out_msg.size());
  SendMsg(sockfd, server_address, out_msg);
  GTL_INFO("server address: {}", server_address.ToString());
  std::string in_msg = RecvMsg(sockfd, server_address, out_msg.size());
  GTL_INFO("server address: {}", server_address.ToString());
  GTL_INFO("in msg {}", in_msg);
  return 0;
}