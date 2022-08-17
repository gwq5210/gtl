#include "gtl/net/poller.h"
#include "gtl/net/socket.h"

#if defined(__APPLE__)
#  include "gtl/net/kqueue.h"
#else
#  include "gtl/net/epoll.h"
#endif

int main(int argc, char* argv[]) {
  GTL_SET_LEVEL(gtl::LogLevel::kDebug);
  const char* address_str = "[::]:9999";
  gtl::SocketAddress server_address(address_str);
  gtl::Socket socket = gtl::Socket::ServerStart(server_address);
  if (socket < 0) {
    return 0;
  }
#if defined(__APPLE__)
  gtl::KqueuePoller poller;
#else
  gtl::EpollPoller poller;
#endif

  GTL_INFO("init begin");
  bool success = poller.Init();
  GTL_INFO("init status: {}", success);
  success = poller.Add(socket, gtl::Poller::kReadable | gtl::Poller::kWritable, (void*)socket.sockfd());
  GTL_INFO("Add status: {}", success);
  while (true) {
    int ret = poller.Wait();
    for (int i = 0; i < ret; ++i) {
      const gtl::Poller::Result& result = poller.GetResult(i);
      int rfd = (uint64_t)result.ptr;
      if (rfd == socket) {
        if (gtl::Poller::EventReadable(result.events)) {
          gtl::SocketAddress peer_address;
          gtl::Socket client_socket = socket.Accept(&peer_address);
          if (client_socket < 0) {
            continue;
          }
          std::string in_msg = client_socket.RecvAll(13);
          GTL_INFO("in msg: {} from {}, recv size: {}, local address: {}, socket peer address: {}", in_msg,
                  peer_address.ToString(), in_msg.size(), client_socket.local_address().ToString(),
                  client_socket.peer_address().ToString());
          ssize_t ret = client_socket.SendAll(in_msg);
          GTL_INFO("send msg ret {}", ret);
          ret = client_socket.SendAll(in_msg);
          GTL_INFO("send msg ret {}", ret);
        }
      }
    }
  }
  return 0;
}