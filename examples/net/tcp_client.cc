#include "gtl/net/socket.h"

#include "gflags/gflags.h"

DEFINE_string(server_address, "127.0.0.1:9999", "server address");

int main(int argc, char* argv[]) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  GTL_SET_LEVEL(gtl::LogLevel::kDebug);
  gtl::SocketAddress server_address(FLAGS_server_address);
  gtl::Socket socket = gtl::Socket::ConnectTo(server_address);
  if (socket < 0) {
    return 0;
  }
  std::string out_msg = "Hello, world!";
  ssize_t ret = socket.SendAll(out_msg);
  GTL_INFO("local address: {}, server address: {}, send size: {}", socket.GetLocalAddr().ToString(), socket.GetPeerAddr().ToString(), ret);
  std::string in_msg = socket.RecvAll(out_msg.size());
  GTL_INFO("recv size: {}, in msg: {}", in_msg.size(), in_msg);
  ret = socket.Send(out_msg);
  GTL_INFO("local address: {}, server address: {}, send size: {}", socket.GetLocalAddr().ToString(), socket.GetPeerAddr().ToString(), ret);
  in_msg = socket.RecvAll(out_msg.size());
  GTL_INFO("recv size: {}, in msg: {}", in_msg.size(), in_msg);
  pause();
  return 0;
}