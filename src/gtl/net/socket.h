/**
 * @file socket.h
 * @author gwq5210 (gwq5210@qq.com)
 * @brief 实现套接字接口
 * @date 2022-08-08
 *
 * @copyright Copyright (c) 2022. All rights reserved.
 */

#pragma once

#include "gtl/logging.h"
#include "gtl/net/socket_address.h"

namespace gtl {

namespace net {

class Socket {
 public:
  static const int kNetBufferSize = 256 * 1024;

  static Socket Create(int domain, int type = SOCK_STREAM);
  static Socket ConnectTo(const SocketAddress& peer_address, int type = SOCK_STREAM);
  static Socket ServerStart(const SocketAddress& address, int type = SOCK_STREAM, int backlog = 65535);

  Socket() = default;
  Socket(int sockfd, int domain, int type = SOCK_STREAM) : sockfd_(sockfd), domain_(domain), type_(type) {}
  void Close() {
    if (sockfd_ >= 0) {
      ::close(sockfd_);
    }
    Clear();
  }
  void Clear() {
    sockfd_ = -1;
    domain_ = AF_UNSPEC;
    type_ = SOCK_STREAM;
    is_connected_ = false;
    peer_address_.Clear();
    local_address_.Clear();
  }

  operator int() const { return sockfd_; }

  void set_sockfd(int fd) { sockfd_ = fd; }
  int sockfd() const { return sockfd_; }
  bool IsValid() const { return sockfd_ >= 0; }
  const SocketAddress& local_address() const { return local_address_; }
  void set_local_address(const SocketAddress& local_address) { local_address_ = local_address; }
  const SocketAddress& peer_address() const { return peer_address_; }
  void set_peer_address(const SocketAddress& peer_address) { peer_address_ = peer_address; }
  bool is_connected() const { return is_connected_; }
  void set_is_connected(bool is_connected) { is_connected_ = is_connected; }

  bool Bind(const SocketAddress& address);
  bool Listen(int backlog = 65535);
  bool Connect(const SocketAddress& peer_address);
  Socket Accept(SocketAddress* peer_address = nullptr);
  bool GetPeerAddr(SocketAddress& peer_address);
  bool GetLocalAddr(SocketAddress& local_address);

  ssize_t Send(const void* buf, size_t buf_len, int flags = 0);
  ssize_t SendAll(const void* buf, size_t buf_len, int flags = 0);
  ssize_t SendTo(const void* buf, size_t buf_len, const SocketAddress& peer_address, int flags = 0);
  ssize_t Send(const std::string& buf, int flags = 0) { return Send(buf.c_str(), buf.size(), flags); }
  ssize_t SendAll(const std::string& buf, int flags = 0) { return SendAll(buf.c_str(), buf.size(), flags); }
  ssize_t SendTo(const std::string& buf, const SocketAddress& peer_address, int flags = 0) {
    return SendTo(buf.c_str(), buf.size(), peer_address, flags);
  }
  ssize_t Recv(void* buf, size_t buf_len, int flags = 0);
  ssize_t RecvAll(void* buf, size_t buf_len, int flags = 0);
  ssize_t RecvFrom(void* buf, size_t buf_len, SocketAddress* peer_address, int flags = 0);
  std::string RecvAll(size_t buf_len, int flags = 0) {
    std::string str(buf_len, '\0');
    ssize_t recv_size = RecvAll((void*)str.data(), str.size(), flags);
    if (recv_size < 0) {
      str.clear();
      return str;
    }
    str.resize(recv_size);
    return str;
  }
  std::string RecvFrom(size_t buf_len, SocketAddress* peer_address, int flags = 0) {
    std::string str(buf_len, '\0');
    ssize_t recv_size = RecvFrom((void*)str.data(), str.size(), peer_address, flags);
    if (recv_size < 0) {
      str.clear();
      return str;
    }
    str.resize(recv_size);
    return str;
  }

 private:
  int sockfd_ = -1;
  int domain_ = AF_UNSPEC;
  int type_ = SOCK_STREAM;
  bool is_connected_ = false;
  SocketAddress peer_address_;
  SocketAddress local_address_;
};

}  // namespace net

}  // namespace gtl