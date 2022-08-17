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

#include "gtl/logging.h"

namespace gtl {

class Poller {
 public:
  Poller() = default;
  virtual ~Poller() { Destroy(); }
  static bool EventReadable(int events) { return events & kReadable; }
  static bool EventWritable(int events) { return events & kWritable; }

  enum Event {
    kReadable = 0x1,
    kWritable = 0x2,
  };

  struct Data {
    int fd = -1;
    int events = 0;
    void* ptr = nullptr;
  };

  struct Result {
    Result(int ev = 0, void* p = nullptr) : events(ev), ptr(p) {}
    int events = 0;
    void* ptr = nullptr;
  };

  virtual bool Init(size_t max_events = 10240) {
    max_events_ = max_events;
    results_ = new Result[max_events_];
  }
  virtual bool Add(int fd, int events, void* ptr = nullptr) = 0;
  virtual bool Del(int fd, int events, void* ptr = nullptr) = 0;
  virtual bool Mod(int fd, int events, void* ptr = nullptr) = 0;
  virtual int Wait(int timeout_ms = -1) = 0;
  const Result& GetResult(int index) const {
    GTL_CHECK(results_ != nullptr && index >= 0 && index < max_events_);
    return results_[index];
  }

 protected:
  void Clear() {
    results_ = nullptr;
    max_events_ = 10240;
  }
  void Destroy() {
    delete[] results_;
    Clear();
  }

  Result* results_ = nullptr;
  size_t max_events_ = 10240;
};

}  // namespace gtl