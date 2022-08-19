#include "gtl/net/poller.h"

#if !defined(__APPLE__)
#  include "gtl/net/epoll.h"
#else
#  include "gtl/net/kqueue.h"
#endif

namespace gtl {

Poller* Poller::CreatePoller() {
#if defined(__APPLE__)
  return new KqueuePoller();
#else
  return new EpollPoller();
#endif
}

int Poller::AddEvents(int old_events, int events) {
  if (Poller::EventReadable(events)) {
    old_events |= Poller::kReadable;
  }
  if (Poller::EventWritable(events)) {
    old_events |= Poller::kWritable;
  }
  return old_events;
}

int Poller::DelEvents(int old_events, int events) {
  if (Poller::EventReadable(events)) {
    old_events &= ~Poller::kReadable;
  }
  if (Poller::EventWritable(events)) {
    old_events &= ~Poller::kWritable;
  }
  return old_events;
}

}  // namespace gtl