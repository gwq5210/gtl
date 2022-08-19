#pragma once

#include "gtl/net/event_handler.h"
#include "gtl/net/poller.h"

namespace gtl {

class Reactor {
 public:
  Reactor() = default;
  Reactor(Reactor&& other) {
    poller_ = other.poller_;
    other.Clear();
  }
  ~Reactor() { Destroy(); }
  Reactor& operator=(Reactor&& other) {
    if (this == &other) {
      return *this;
    }
    Destroy();
    poller_ = other.poller_;
    other.Clear();
    return *this;
  }

  Poller* poller() { return poller_; }
  const Poller* poller() const { return poller_; }

  void set_event_callback(std::function<void(int, void*)>&& func) { poller_->set_event_callback(std::move(func)); }
  void set_wait_callback(std::function<void(int)>&& func) { poller_->set_wait_callback(std::move(func)); }

  virtual bool Init();
  virtual bool Dispatch();
  virtual bool HandleEvents(int events, void* ptr);

 private:
  Reactor(const Reactor& other) = delete;
  Reactor& operator=(const Reactor& other) = delete;

  void Clear() { poller_ = nullptr; }

  void Destroy() {
    delete poller_;
    Clear();
  }

  Poller* poller_ = nullptr;
};

}  // namespace gtl