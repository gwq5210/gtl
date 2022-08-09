#include "gtl/net/socket_util.h"

#include <sys/socket.h>
#include <unistd.h>

#include "gtl/logging.h"

static const int kNetBufferSize = 256 * 1024;

int ClientConnect(const SocketAddress& socket_address, int type /* = SOCK_STREAM */) {
  if (socket_address.socklen() == 0) {
    GTL_ERROR("socket address is empty");
    return -1;
  }
  int sockfd = socket(socket_address.family(), type, 0);
  if (sockfd < 0) {
    GTL_ERROR("create socket file descriptor failed! errno:{}, errmsg:{}", errno, strerror(errno));
    return -1;
  }

  if (connect(sockfd, &socket_address.addr(), socket_address.socklen()) != 0) {
    GTL_ERROR("connect address({}) failed! errno:{}, errmsg:{}", socket_address.ToString(), errno, strerror(errno));
    close(sockfd);
    return -1;
  }
  return sockfd;
}

int ServerStart(const SocketAddress& socket_address, int type /* = SOCK_STREAM */) {
  if (socket_address.socklen() == 0) {
    GTL_ERROR("socket address is empty");
    return -1;
  }
  int sockfd = socket(socket_address.family(), type, 0);
  if (sockfd < 0) {
    GTL_ERROR("create socket file descriptor failed! errno:{}, errmsg:{}", errno, strerror(errno));
    return -1;
  }

  if (bind(sockfd, &socket_address.addr(), socket_address.socklen()) != 0) {
    GTL_ERROR("bind address({}) failed! errno:{}, errmsg:{}", socket_address.ToString(), errno, strerror(errno));
    close(sockfd);
    return -1;
  }

  if (type == SOCK_STREAM || type == SOCK_SEQPACKET) {
    if (listen(sockfd, 65535) != 0) {
      GTL_ERROR("listen address({}) failed! errno:{}, errmsg:{}", socket_address.ToString(), errno, strerror(errno));
      close(sockfd);
      return -1;
    }
  }

  return sockfd;
}

ssize_t SendMsg(int sockfd, const std::string& msg) {
  if (msg.empty()) {
    return 0;
  }
  ssize_t send_len = 0;
  do {
    ssize_t len = write(sockfd, msg.c_str(), msg.size());
    if (len == -1) {
      return -1;
    }
    send_len += len;
  } while (send_len < msg.size());
  return send_len;
}

std::string RecvMsg(int sockfd, size_t msg_size) {
  static thread_local char msg_buf[kNetBufferSize];
  std::string msg;
  msg.reserve(msg_size);
  do {
    ssize_t len = read(sockfd, msg_buf, sizeof(msg_buf));
    if (len == -1) {
      return "";
    }
    msg.append(msg_buf, len);
  } while (msg.size() < msg_size);
  return msg;
}

ssize_t SendMsg(int sockfd, const SocketAddress& socket_address, const std::string& msg) {
  ssize_t ret = sendto(sockfd, msg.c_str(), msg.size(), 0, &socket_address.addr(), socket_address.socklen());
  if (ret == -1) {
    GTL_ERROR("sendto {} failed, errno:{}, errmsg:{}", socket_address.ToString(), errno, strerror(errno));
    return -1;
  }
  return ret;
}

std::string RecvMsg(int sockfd, SocketAddress& socket_address, size_t msg_size) {
  std::string msg(msg_size, '\0');
  socket_address.Clear();
  ssize_t real_size =
      recvfrom(sockfd, (void*)msg.data(), msg.size(), 0, &socket_address.addr(), &socket_address.socklen());
  if (real_size == -1) {
    return "";
  }
  msg.resize(real_size);
  return msg;
}

int Accept(int server_fd, SocketAddress& peer_address) {
  int client_fd = accept(server_fd, &peer_address.addr(), &peer_address.socklen());
  if (client_fd == -1) {
    GTL_ERROR("accept failed: errno:{}, errmsg:{}", errno, strerror(errno));
    return -1;
  }
  GTL_INFO("new connection from address {}", peer_address.ToString());
  return client_fd;
}

bool GetLocalAddr(int sockfd, SocketAddress& socket_address) {
  socket_address.Clear();
  if (getsockname(sockfd, &socket_address.addr(), &socket_address.socklen()) != 0) {
    GTL_ERROR("getsockname failed! errno:{}, errmsg:{}", errno, strerror(errno));
    return false;
  }
  return true;
}

bool GetPeerAddr(int sockfd, SocketAddress& socket_address) {
  socket_address.Clear();
  if (getpeername(sockfd, &socket_address.addr(), &socket_address.socklen()) != 0) {
    GTL_ERROR("getpeername failed! errno:{}, errmsg:{}", errno, strerror(errno));
    return false;
  }
  return true;
}