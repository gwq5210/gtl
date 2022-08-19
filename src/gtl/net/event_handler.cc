#include "gtl/net/event_handler.h"

#include "gtl/net/reactor.h"

namespace gtl {

bool EventHandler::RegsiterEvents(int events) {
  int add_events = 0;
  if (!Poller::EventReadable(events_) && Poller::EventReadable(events)) {
    add_events |= Poller::kReadable;
  }
  if (!Poller::EventWritable(events_) && Poller::EventWritable(events)) {
    add_events |= Poller::kWritable;
  }
  if (add_events) {
    reactor_->poller()->Add(GetFd(), add_events, this);
    events_ = Poller::AddEvents(events_, events);
  }
  return true;
}

bool EventHandler::ModifyEvents(int events) {
  if (events == 0) {
    return RemoveAllEvents();
  } else {
    reactor_->poller()->Mod(GetFd(), events_, events, this);
    events_ = Poller::AddEvents(0, events);
  }
  return true;
}

bool EventHandler::RemoveEvents(int events) {
  int remove_events = 0;
  if (Poller::EventReadable(events_) && Poller::EventReadable(events)) {
    remove_events |= Poller::kReadable;
  }
  if (Poller::EventWritable(events_) && Poller::EventWritable(events)) {
    remove_events |= Poller::kWritable;
  }
  if (remove_events) {
    reactor_->poller()->Del(GetFd(), remove_events, this);
  }
  events_ = Poller::DelEvents(events_, events);
  return true;
}

bool EventHandler::RemoveAllEvents() {
  if (events_) {
    reactor_->poller()->Del(GetFd(), events_, this);
    events_ = 0;
  }
  return true;
}

}  // namespace gtl