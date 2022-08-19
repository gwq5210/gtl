#pragma once

#include "gtl/net/event_handler.h"
#include "gtl/net/socket.h"

namespace gtl {

class NetEventHandler : public EventHandler {
 public:
  static const int kMaxBufferSize = 128 * 1024 * 1024;

  NetEventHandler(Reactor* reactor) : EventHandler(reactor), buffer_(kMaxBufferSize, '\0') {}

  void set_message_complete(std::function<ssize_t(const void*, size_t)>&& func) { message_complete_ = std::move(func); }
  void set_message_handle(std::function<bool(const void*, size_t)>&& func) { message_handle_ = std::move(func); }

  virtual int GetFd() const override { return socket_.sockfd(); }
  virtual bool HandleReadEvents() override;
  virtual bool HandleWriteEvents() override;

 private:
  size_t msg_size_ = 0;
  size_t expect_len_ = 0;
  std::string buffer_;
  Socket socket_;
  std::function<ssize_t(const void*, size_t)> message_complete_;
  std::function<bool(const void*, size_t)> message_handle_;
};

}  // namespace gtl