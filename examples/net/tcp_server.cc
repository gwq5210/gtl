#include "gtl/net/socket.h"

#include "gflags/gflags.h"

DEFINE_string(server_address, "127.0.0.1:9999", "server address");

int main(int argc, char* argv[]) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  GTL_SET_LEVEL(gtl::LogLevel::kDebug);
  gtl::SocketAddress server_address(FLAGS_server_address);
  gtl::Socket socket = gtl::Socket::ServerStart(server_address);
  if (socket < 0) {
    return 0;
  }
  socket.SetNonBlocking(false);
  while (true) {
    gtl::SocketAddress peer_address;
    gtl::Socket client_socket = socket.Accept(&peer_address);
    if (client_socket < 0) {
      continue;
    }
    std::string in_msg = client_socket.RecvAll(13);
    GTL_INFO("in msg: {} from {}, recv size: {}, local address: {}, socket peer address: {}", in_msg,
             peer_address.ToString(), in_msg.size(), client_socket.GetLocalAddr().ToString(),
             client_socket.GetPeerAddr().ToString());
    ssize_t ret = client_socket.SendAll(in_msg);
    GTL_INFO("send msg ret {}", ret);
    ret = client_socket.SendAll(in_msg);
    GTL_INFO("send msg ret {}", ret);
  }
  return 0;
}