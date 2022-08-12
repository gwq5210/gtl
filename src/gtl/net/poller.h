/**
 * @file poller.h
 * @author gwq5210 (gwq5210@qq.com)
 * @brief 网络poll的封装
 * @date 2022-08-09
 *
 * @copyright Copyright (c) 2022. All rights reserved.
 */

#pragma once

#include <vector>

namespace gtl {

enum class PollerEvent {
  kReadable,
  kWritable,
  kError,
  kHangUp,
};

class PollerData {
 public:
  int fd;
  PollerEvent event;
};

struct EventResult {
  int fd_ = -1;
  PollerEvent poller_event;
  void* custom_data = nullptr;
};

class IPoller {
 public:
  virtual bool Init() = 0;
  virtual bool Add(int fd, PollerEvent event) = 0;
  virtual bool Del(int fd, PollerEvent event) = 0;
  virtual bool Mod(int fd, PollerEvent event) = 0;
  virtual bool Wait(std::vector<EventResult>& results, int timeout_ms = -1) = 0;
};

}  // namespace gtl