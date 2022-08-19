#include "gtl/net/reactor.h"

namespace gtl {

bool Reactor::Init() {
  poller_ = Poller::CreatePoller();
  poller_->Init();
  poller_->set_event_callback([this](int events, void* ptr) { HandleEvents(events, ptr); });
  return true;
}

bool Reactor::Dispatch() {
  while (true) {
    poller_->Dispatch();
  }
  return 0;
}

bool Reactor::HandleEvents(int events, void* ptr) {
  EventHandler* handler = static_cast<EventHandler*>(ptr);
  handler->HandleEvents(events);
  return true;
}

}  // namespace gtl