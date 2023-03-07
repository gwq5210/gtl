#include "gtl/net/socket.h"

#include "gflags/gflags.h"

DECLARE_bool(reuse_addr);
DECLARE_bool(reuse_port);

DEFINE_bool(listen_first, false, "listen server address1");
DEFINE_string(server_address1, "127.0.0.1:9999", "server address1");
DEFINE_string(server_address2, "127.0.0.1:9999", "server address1");

gtl::Socket BindAndListen(const std::string& msg, const std::string& server_address) {
  GTL_INFO("{} BindAndListen {} started", msg, server_address);
  gtl::SocketAddress socket_address(server_address);
  gtl::Socket socket = gtl::Socket::Create(socket_address.family());
  if (FLAGS_reuse_addr) {
    socket.EnableReuseAddr();
  } else {
    socket.DisableReuseAddr();
  }
  if (FLAGS_reuse_port) {
    socket.EnableReusePort();
  } else {
    socket.DisableReusePort();
  }
  if (!socket.Bind(socket_address)) {
    return socket;
  }
  GTL_INFO("bind server address {} sucessfully", server_address);
  if (!FLAGS_listen_first) {
    return socket;
  }
  if (!socket.Listen()) {
    return socket;
  }
  GTL_INFO("listen server address {} sucessfully", server_address);
  return socket;
}

int main(int argc, char* argv[]) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  GTL_SET_LEVEL(gtl::LogLevel::kDebug);
  gtl::Socket socket1 = BindAndListen("first", FLAGS_server_address1);
  gtl::Socket socket2 = BindAndListen("second", FLAGS_server_address2);

  pause();
  return 0;
}