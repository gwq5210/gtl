#include "gtl/net/socket.h"

namespace gtl {

Socket Socket::Create(int domain, int type /* = SOCK_STREAM */) {
  int fd = ::socket(domain, type, 0);
  if (fd < 0) {
    GTL_ERROR("create socket failed, errno:{}, errmsg:{}", errno, strerror(errno));
  }
  return Socket(fd, domain, type);
}

Socket Socket::ConnectTo(const SocketAddress& peer_address, int type /* = SOCK_STREAM */) {
  Socket socket = Socket::Create(peer_address.family(), type);
  if (!socket.IsValid()) {
    return socket;
  }
  bool is_connected = socket.Connect(peer_address);
  if (!is_connected) {
    socket.Close();
    return socket;
  }
  return socket;
}

Socket Socket::ServerStart(const SocketAddress& address, int type /* = SOCK_STREAM */, int backlog /* = 65535 */) {
  Socket socket = Socket::Create(address.family(), type);
  if (!socket.IsValid()) {
    return socket;
  }

  if (!socket.Bind(address)) {
    return socket;
  }

  if (!socket.Listen(backlog)) {
    return socket;
  }

  return socket;
}

bool Socket::Bind(const SocketAddress& address) {
  if (::bind(sockfd_, &address.addr(), address.socklen()) != 0) {
    GTL_ERROR("bind address({}) failed! errno:{}, errmsg:{}", address.ToString(), errno, strerror(errno));
    Close();
    return false;
  }
  return true;
}

bool Socket::Listen(int backlog /* = 65535 */) {
  if (!(type_ == SOCK_STREAM || type_ == SOCK_SEQPACKET)) {
    return true;
  }
  if (::listen(sockfd_, backlog) == -1) {
    GTL_ERROR("listen failed! errno:{}, errmsg:{}", errno, strerror(errno));
    Close();
    return false;
  }
  return true;
}

bool Socket::Connect(const SocketAddress& peer_address) {
  if (::connect(sockfd_, &peer_address.addr(), peer_address.socklen()) == -1) {
    GTL_ERROR("connect to address:{} failed, errno:{}, errmsg:{}", peer_address.ToString(), errno, strerror(errno));
    Close();
    return false;
  }
  return true;
}

Socket Socket::Accept(SocketAddress* peer_address /* = nullptr*/) {
  SocketAddress address;
  int client_fd = ::accept(sockfd_, &address.addr(), &address.socklen());
  if (client_fd == -1) {
    GTL_ERROR("accept failed, errno:{}, errmsg:{}", errno, strerror(errno));
    return Socket();
  }
  GTL_INFO("new connection from address {}", address.ToString());
  if (peer_address) {
    *peer_address = address;
  }
  return Socket(client_fd, domain_, type_);
}

bool Socket::GetLocalAddr(SocketAddress& local_address) {
  if (::getsockname(sockfd_, &local_address.addr(), &local_address.socklen()) != 0) {
    GTL_ERROR("getsockname failed! errno:{}, errmsg:{}", errno, strerror(errno));
    local_address.Clear();
    return false;
  }
  return true;
}

bool Socket::GetPeerAddr(SocketAddress& peer_address) {
  if (::getpeername(sockfd_, &peer_address.addr(), &peer_address.socklen()) != 0) {
    GTL_ERROR("getpeername failed! errno:{}, errmsg:{}", errno, strerror(errno));
    peer_address.Clear();
    return false;
  }
  return true;
}

ssize_t Socket::Send(const void* buf, size_t buf_len, int flags /* = 0*/) {
  if (buf_len == 0) {
    return 0;
  }
  if (buf == nullptr) {
    GTL_ERROR("buf is nullptr");
    return -1;
  }
  ssize_t len = ::send(sockfd_, buf, buf_len, flags);
  if (len == -1) {
    GTL_ERROR("send failed, errno:{}, errmsg:{}", errno, strerror(errno));
    return -1;
  }
  return len;
}

ssize_t Socket::SendAll(const void* buf, size_t buf_len, int flags /* = 0*/) {
  if (buf_len == 0) {
    return 0;
  }
  if (buf == nullptr) {
    GTL_ERROR("buf is nullptr");
    return -1;
  }
  ssize_t send_len = 0;
  do {
    ssize_t len = ::send(sockfd_, buf, buf_len, flags);
    if (len == -1) {
      GTL_ERROR("send failed, errno:{}, errmsg:{}", errno, strerror(errno));
      return -1;
    }
    send_len += len;
  } while (send_len < buf_len);
  return send_len;
}

ssize_t Socket::SendTo(const void* buf, size_t buf_len, const SocketAddress& peer_address, int flags /* = 0*/) {
  if (buf_len == 0) {
    return 0;
  }
  if (buf == nullptr) {
    GTL_ERROR("buf is nullptr");
    return -1;
  }
  ssize_t ret = ::sendto(sockfd_, buf, buf_len, flags, &peer_address.addr(), peer_address.socklen());
  if (ret == -1) {
    GTL_ERROR("sendto {} failed, errno:{}, errmsg:{}", peer_address.ToString(), errno, strerror(errno));
    return -1;
  }
  return ret;
}

ssize_t Socket::Recv(void* buf, size_t buf_len, int flags /* = 0*/) {
  if (buf_len == 0) {
    return 0;
  }
  if (buf == nullptr) {
    GTL_ERROR("buf is nullptr");
    return -1;
  }
  ssize_t len = ::recv(sockfd_, buf, buf_len, flags);
  if (len == -1) {
    GTL_ERROR("recv failed, errno:{}, errmsg:{}", errno, strerror(errno));
    return -1;
  }
  return len;
}

ssize_t Socket::RecvAll(void* buf, size_t buf_len, int flags /* = 0*/) {
  if (buf_len == 0) {
    return 0;
  }
  if (buf == nullptr) {
    GTL_ERROR("buf is nullptr");
    return -1;
  }
  ssize_t read_len = 0;
  do {
    ssize_t len = ::recv(sockfd_, reinterpret_cast<char*>(buf) + read_len, buf_len - read_len, flags);
    if (len == -1) {
      GTL_ERROR("recv failed, errno:{}, errmsg:{}", errno, strerror(errno));
      return -1;
    }
    read_len += len;
  } while (read_len < buf_len);
  return read_len;
}

ssize_t Socket::RecvFrom(void* buf, size_t buf_len, SocketAddress* peer_address, int flags /* = 0*/) {
  if (buf_len == 0) {
    return 0;
  }
  if (buf == nullptr) {
    GTL_ERROR("buf is nullptr");
    return -1;
  }
  SocketAddress address;
  ssize_t recv_size = ::recvfrom(sockfd_, buf, buf_len, 0, &address.addr(), &address.socklen());
  if (recv_size == -1) {
    GTL_ERROR("recvfrom {} failed, errno:{}, errmsg:{}", address.ToString(), errno, strerror(errno));
    return -1;
  }
  if (peer_address) {
    *peer_address = address;
  }
  return recv_size;
}

}  // namespace gtl