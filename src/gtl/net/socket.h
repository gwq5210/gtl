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
#include "gtl/net/socket_util.h"

class Socket {
 public:
  Socket() = default;
  explicit Socket(int sockfd) { SetFd(sockfd); }
  void Clear() {
    if (sockfd_ >= 0) {
      close(sockfd_);
    }
    local_address_.Clear();
    peer_address_.Clear();
  }
  bool Connect(const SocketAddress& server_address, int type = SOCK_STREAM);  // for client
  bool SetFd(int sockfd);
  ssize_t SendMsg(const std::string& msg);
  std::string RecvMsg(size_t msg_size);
  const SocketAddress& local_address() const { return local_address_; }
  const SocketAddress& peer_address() const { return peer_address_; }

 private:
  int sockfd_ = -1;
  SocketAddress local_address_;
  SocketAddress peer_address_;
};