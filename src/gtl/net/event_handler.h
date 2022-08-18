#pragma once

#include "gtl/net/poller.h"

namespace gtl {

class Reactor;

class EventHandler {
 public:
  EventHandler(Reactor* reactor, int events) {}

  virtual int GetFd() const = 0;
  int events() const { return events_; }
  void set_events(int events) {
    events_ = 0;
    if (Poller::EventReadable(events)) {
      events_ |= Poller::kReadable;
    }
    if (Poller::EventWritable(events)) {
      events_ |= Poller::kWritable;
    }
  }

 private:
  EventHandler(const EventHandler& other) = delete;
  EventHandler& operator=(const EventHandler& other) = delete;

  Reactor* recator_ = nullptr;
  int events_ = 0;
};

}  // namespace gtl