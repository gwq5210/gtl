#include "gtl/net/reactor.h"

namespace gtl {

bool Reactor::Init() {
  poller_ = Poller::CreatePoller();
  poller_->Init();
  return true;
}

bool Reactor::Dispatch() {
  while (true) {
    poller_->Dispatch();
  }
  return 0;
}

}  // namespace gtl