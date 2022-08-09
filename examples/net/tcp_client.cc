#include "gtl/net/socket.h"

int main(int argc, char* argv[]) {
  const char* address_str = "127.0.0.1:9999";
  SocketAddress server_address(address_str);
  Socket socket;
  if (!socket.Connect(server_address)) {
    return 0;
  }
  std::string out_msg = "Hello, world!";
  ssize_t ret = socket.SendMsg(out_msg);
  GTL_INFO("local address: {}, server address: {}, send size: {}", socket.local_address().ToString(),
           socket.peer_address().ToString(), ret);
  std::string in_msg = socket.RecvMsg(out_msg.size());
  GTL_INFO("recv size: {}, in msg: {}", in_msg.size(), in_msg);
  return 0;
}