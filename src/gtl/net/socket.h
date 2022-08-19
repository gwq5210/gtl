/**
 * @file socket.h
 * @author gwq5210 (gwq5210@qq.com)
 * @brief 实现套接字接口
 * @date 2022-08-08
 *
 * @copyright Copyright (c) 2022. All rights reserved.
 */

#pragma once

#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>

#include "gtl/logging.h"
#include "gtl/net/socket_address.h"
#include "gtl/util/fd_util.h"

namespace gtl {

class Socket {
 public:
  static const int kNetBufferSize = 256 * 1024;

  static std::string DomainName(int domain);
  static std::string TypeName(int type);
  static std::string GetSocketInfo(const Socket& socket);
  static Socket Create(int domain, int type = SOCK_STREAM);
  static Socket ConnectTo(const SocketAddress& peer_address, int type = SOCK_STREAM);
  static Socket ServerStart(const SocketAddress& address, int type = SOCK_STREAM, int backlog = 65535);

  Socket() = default;
  Socket(int sockfd, int domain, int type = SOCK_STREAM) : sockfd_(sockfd), domain_(domain), type_(type) {}
  Socket(Socket&& other) {
    sockfd_ = other.sockfd_;
    domain_ = other.domain_;
    type_ = other.type_;

    other.Clear();
  }
  Socket& operator=(Socket&& other) {
    if (this == &other) {
      return *this;
    }

    Close();

    sockfd_ = other.sockfd_;
    domain_ = other.domain_;
    type_ = other.type_;

    other.Clear();
    return *this;
  }
  ~Socket() { Close(); }

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
  }

  std::string ToString() const {
    if (IsValid()) {
      return fmt::format("socket[{}] {} {}", sockfd(), DomainName(domain_), TypeName(type_));
    } else {
      return "invalid_socket";
    }
  }

  operator int() const { return sockfd_; }

  void set_sockfd(int fd) { sockfd_ = fd; }
  int sockfd() const { return sockfd_; }
  void set_domain(int domain) { domain_ = domain; }
  int domain() const { return domain_; }
  int set_type(int type) { type_ = type; }
  int type() const { return type_; }
  bool IsValid() const { return sockfd_ >= 0; }

  bool Bind(const SocketAddress& address) const;
  bool Listen(int backlog = 65535) const;
  bool Connect(const SocketAddress& peer_address) const;
  Socket Accept(SocketAddress* peer_address = nullptr) const;
  bool GetPeerAddr(SocketAddress& peer_address) const;
  bool GetLocalAddr(SocketAddress& local_address) const;
  SocketAddress GetPeerAddr() const {
    SocketAddress peer_address;
    GetPeerAddr(peer_address);
    return peer_address;
  }
  SocketAddress GetLocalAddr() const {
    SocketAddress local_address;
    GetLocalAddr(local_address);
    return local_address;
  }

  ssize_t Send(const void* buf, size_t buf_len, int flags = 0) const;
  ssize_t SendAll(const void* buf, size_t buf_len, int flags = 0) const;
  ssize_t SendTo(const void* buf, size_t buf_len, const SocketAddress& peer_address, int flags = 0) const;
  ssize_t Send(const std::string& buf, int flags = 0) const { return Send(buf.c_str(), buf.size(), flags); }
  ssize_t SendAll(const std::string& buf, int flags = 0) const { return SendAll(buf.c_str(), buf.size(), flags); }
  ssize_t SendTo(const std::string& buf, const SocketAddress& peer_address, int flags = 0) const {
    return SendTo(buf.c_str(), buf.size(), peer_address, flags);
  }
  ssize_t Recv(void* buf, size_t buf_len, int flags = 0) const;
  ssize_t RecvAll(void* buf, size_t buf_len, int flags = 0) const;
  ssize_t RecvFrom(void* buf, size_t buf_len, SocketAddress* peer_address, int flags = 0) const;
  std::string RecvAll(size_t buf_len, int flags = 0) const {
    std::string str(buf_len, '\0');
    ssize_t recv_size = RecvAll((void*)str.data(), str.size(), flags);
    if (recv_size < 0) {
      str.clear();
      return str;
    }
    str.resize(recv_size);
    return str;
  }
  std::string RecvFrom(size_t buf_len, SocketAddress* peer_address, int flags = 0) const {
    std::string str(buf_len, '\0');
    ssize_t recv_size = RecvFrom((void*)str.data(), str.size(), peer_address, flags);
    if (recv_size < 0) {
      str.clear();
      return str;
    }
    str.resize(recv_size);
    return str;
  }
  bool GetSockOpt(int level, int optname, void* optval, socklen_t* optlen) const {
    int ret = ::getsockopt(sockfd_, level, optname, optval, optlen);
    if (ret != 0) {
      GTL_TRACE("getsockopt level:{}, optname:{} failed, errno:{}, errmsg:{}", level, optname, errno, strerror(errno));
      return false;
    }
    return true;
  }
  bool SetSockOpt(int level, int optname, const void* optval, socklen_t optlen) const {
    int ret = ::setsockopt(sockfd_, level, optname, optval, optlen);
    if (ret != 0) {
      GTL_TRACE("setsockopt level:{}, optname:{} failed, errno:{}, errmsg:{}", level, optname, errno, strerror(errno));
      return false;
    }
    return true;
  }
  bool SetSockOptInt(int level, int optname, int opt) const {
    return SetSockOpt(level, optname, static_cast<const void*>(&opt), static_cast<socklen_t>(sizeof(opt)));
  }
  int GetSockOptInt(int level, int optname) const {
    int opt = 0;
    socklen_t optlen = sizeof(int);
    GetSockOpt(level, optname, static_cast<void*>(&opt), &optlen);
    return opt;
  }
  bool EnableSockOpt(int level, int optname) const { return SetSockOptInt(level, optname, 1); }
  bool DisableSockOpt(int level, int optname) const { return SetSockOptInt(level, optname, 0); }
  bool IsKeepAlive() const { return GetSockOptInt(SOL_SOCKET, SO_KEEPALIVE); }
  bool EnableKeepAlive() const { return EnableSockOpt(SOL_SOCKET, SO_KEEPALIVE); }
  bool DisableKeepAlive() const { return DisableSockOpt(SOL_SOCKET, SO_KEEPALIVE); }
  bool IsReuseAddr() const { return GetSockOptInt(SOL_SOCKET, SO_REUSEADDR); }
  bool EnableReuseAddr() const { return EnableSockOpt(SOL_SOCKET, SO_REUSEADDR); }
  bool DisableReuseAddr() const { return DisableSockOpt(SOL_SOCKET, SO_REUSEADDR); }
  bool IsReusePort() const { return GetSockOptInt(SOL_SOCKET, SO_REUSEPORT); }
  bool EnableReusePort() const { return EnableSockOpt(SOL_SOCKET, SO_REUSEPORT); }
  bool DisableReusePort() const { return DisableSockOpt(SOL_SOCKET, SO_REUSEPORT); }
  bool IsNoDelay() const { return GetSockOptInt(IPPROTO_TCP, TCP_NODELAY); }
  bool EnableNoDelay() const { return EnableSockOpt(IPPROTO_TCP, TCP_NODELAY); }
  bool DisableNoDelay() const { return DisableSockOpt(IPPROTO_TCP, TCP_NODELAY); }
  int GetRecvBufferSize() const { return GetSockOptInt(SOL_SOCKET, SO_RCVBUF); }
  bool SetRecvBufferSize(int sz) const { return SetSockOptInt(SOL_SOCKET, SO_RCVBUF, sz); }
  int GetSendBufferSize() const { return GetSockOptInt(SOL_SOCKET, SO_SNDBUF); }
  bool SetSendBufferSize(int sz) const { return SetSockOptInt(SOL_SOCKET, SO_SNDBUF, sz); }
  bool IsNonBlocking() const { return IsFdNonBlocking(sockfd_); }
  bool SetNonBlocking(int non_blocking = true) const { return SetFdNonBlocking(sockfd_, non_blocking); }

 private:
  Socket(const Socket& other) = delete;
  Socket& operator=(const Socket& other) = delete;

  int sockfd_ = -1;
  int domain_ = AF_UNSPEC;
  int type_ = SOCK_STREAM;
};

inline bool operator==(const Socket& lhs, const Socket& rhs) {
  return lhs.sockfd() == rhs.sockfd() && lhs.domain() == rhs.domain() && lhs.type() == lhs.type();
}

}  // namespace gtl