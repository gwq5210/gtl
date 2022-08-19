#pragma once

#include "gtl/net/poller.h"

namespace gtl {

class Reactor;

class EventHandler {
 public:
  EventHandler(Reactor* reactor) : reactor_(reactor) {}

  int events() const { return events_; }

  bool RegsiterEvents(int events);
  bool ModifyEvents(int events);
  bool RemoveEvents(int events);
  bool RemoveAllEvents();

  virtual int GetFd() const = 0;
  virtual bool HandleReadEvents() = 0;
  virtual bool HandleWriteEvents() = 0;

  virtual bool HandleEvents(int revents) {
    if (Poller::EventReadable(revents)) {
      HandleReadEvents();
    }
    if (Poller::EventWritable(revents)) {
      HandleWriteEvents();
    }
    return true;
  }

 private:
  EventHandler(const EventHandler& other) = delete;
  EventHandler& operator=(const EventHandler& other) = delete;

  Reactor* reactor_ = nullptr;
  int events_ = 0;
};

}  // namespace gtl