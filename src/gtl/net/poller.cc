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

}  // namespace gtl