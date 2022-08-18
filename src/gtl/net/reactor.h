#pragma once

#include "gtl/net/event_handler.h"
#include "gtl/net/poller.h"

namespace gtl {

class Reactor {
 public:
  void set_handle_event_function(std::function<void(int, void*)>&& func) { handle_event_ = std::move(func); }

  Poller* poller() { return poller_; }
  const Poller* poller() const { return poller_; }

  virtual bool Init();
  virtual bool RegisterHandler(EventHandler* handler);
  virtual bool RemoveHandler(EventHandler* handler);
  virtual bool Dispatch();

 private:
  Poller* poller_ = nullptr;
  std::function<void(int, void*)> handle_event_;
};

}  // namespace gtl