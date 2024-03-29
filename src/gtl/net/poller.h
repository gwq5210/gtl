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
  static bool EventReadable(int events) { return events & kReadable; }
  static bool EventWritable(int events) { return events & kWritable; }
  static int AddEvents(int old_events, int events);
  static int DelEvents(int old_events, int events);
  static Poller* CreatePoller();

  Poller() = default;
  Poller(Poller&& other) {
    results_ = other.results_;
    max_events_ = other.max_events_;
    wait_callback_ = std::move(other.wait_callback_);
    event_callback_ = std::move(other.event_callback_);

    other.Clear();
  }
  virtual ~Poller() { Destroy(); }

  Poller& operator=(Poller&& other) {
    if (this == &other) {
      return *this;
    }

    Destroy();

    results_ = other.results_;
    max_events_ = other.max_events_;
    wait_callback_ = std::move(other.wait_callback_);
    event_callback_ = std::move(other.event_callback_);

    other.Clear();
    return *this;
  }

  enum Event {
    kReadable = 0x1,
    kWritable = 0x2,
  };

  struct Result {
    Result(int ev = 0, void* p = nullptr) : events(ev), ptr(p) {}
    int events = 0;
    void* ptr = nullptr;
  };

  void set_event_callback(std::function<void(int, void*)>&& func) { event_callback_ = std::move(func); }
  void set_wait_callback(std::function<void(int)>&& func) { wait_callback_ = std::move(func); }

  virtual bool Init(size_t max_events = 10240) {
    max_events_ = max_events;
    results_ = new Result[max_events_];
  }
  virtual bool Add(int fd, int events, void* ptr = nullptr) = 0;
  virtual bool Del(int fd, int events, void* ptr = nullptr) = 0;
  virtual bool Mod(int fd, int old_events, int events, void* ptr = nullptr) = 0;
  virtual int Wait(int timeout_ms = -1) = 0;
  const Result& GetResult(int index) const {
    GTL_CHECK(results_ != nullptr && index >= 0 && index < max_events_);
    return results_[index];
  }
  void Dispatch(int timeout_ms = -1) {
    int event_num = Wait(timeout_ms);
    if (wait_callback_) {
      wait_callback_(event_num);
    }

    if (event_callback_) {
      for (int i = 0; i < event_num; ++i) {
        const Result& result = GetResult(i);
        event_callback_(result.events, result.ptr);
      }
    }
  }

 protected:
  void Clear() {
    results_ = nullptr;
    max_events_ = 10240;
    wait_callback_ = nullptr;
    event_callback_ = nullptr;
  }
  void Destroy() {
    delete[] results_;
    Clear();
  }

  Result* results_ = nullptr;
  size_t max_events_ = 10240;
  std::function<void(int)> wait_callback_;
  std::function<void(int, void*)> event_callback_;
};

}  // namespace gtl

#if !defined(__APPLE__)
#  include "gtl/net/epoll.h"
#else
#  include "gtl/net/kqueue.h"
#endif