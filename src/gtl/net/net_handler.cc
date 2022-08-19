#include "gtl/net/net_handler.h"

namespace gtl {

bool NetEventHandler::HandleReadEvents() {
  ssize_t recv_len = socket_.RecvAll((char*)buffer_.data() + msg_size_, buffer_.size() - msg_size_);
  if (recv_len >= 0) {
    msg_size_ += recv_len;
  }
  ssize_t expect_len = message_complete_(buffer_.data(), msg_size_);
  if (expect_len < 0) {
    msg_size_ = 0;
    expect_len_ = 0;
    return false;
  } else if (expect_len == 0) {
    message_handle_(buffer_.data(), msg_size_);
    ModifyEvents(Poller::kWritable);
  }
  return true;
}

bool NetEventHandler::HandleWriteEvents() {
  
  msg_size_ = 0;
  expect_len_ = 0;
  ModifyEvents(Poller::kWritable);
  return true;
}

}  // namespace gtl