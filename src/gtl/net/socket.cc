#include "gtl/net/socket.h"

#include "gtl/net/socket_util.h"

bool Socket::Connect(const SocketAddress& server_address, int type /* = SOCK_STREAM */) {
  Clear();
  peer_address_ = server_address;
  sockfd_ = ClientConnect(server_address, type);
  if (sockfd_ < 0) {
    return false;
  }
  GetLocalAddr(sockfd_, local_address_);
  return true;
}

bool Socket::SetFd(int sockfd) {
  Clear();
  sockfd_ = sockfd;
  GetLocalAddr(sockfd_, local_address_);
  GetPeerAddr(sockfd_, peer_address_);
  return true;
}

ssize_t Socket::SendMsg(const std::string& msg) {
  ssize_t send_len = ::SendMsg(sockfd_, msg);
  if (send_len != msg.size()) {
    GTL_ERROR("write to {} failed! errno:{}, errmsg:{}, send_len:{}/{}", peer_address_.ToString(), errno,
              strerror(errno), send_len, msg.size());
  }
  return send_len;
}

std::string Socket::RecvMsg(size_t msg_size) {
  std::string msg = ::RecvMsg(sockfd_, msg_size);
  if (msg.size() != msg_size) {
    GTL_ERROR("read from {} failed! errno:{}, errmsg:{}, recv_len:{}/{}", peer_address_.ToString(), errno,
              strerror(errno), msg.size(), msg_size);
  }
  return msg;
}