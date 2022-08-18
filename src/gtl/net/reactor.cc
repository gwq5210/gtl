#include "gtl/net/reactor.h"

namespace gtl {

bool Reactor::Init() {
  poller_ = Poller::CreatePoller();
  poller_->Init();
  return true;
}

bool Reactor::RegisterHandler(EventHandler* handler) {
  return poller_->Add(handler->GetFd(), handler->events(), handler);
}

bool Reactor::RemoveHandler(EventHandler* handler) {
  return poller_->Del(handler->GetFd(), handler->events(), handler);
}

bool Reactor::Dispatch() {
  while (true) {
    poller_->Dispatch();
  }
  return 0;
}

}  // namespace gtl