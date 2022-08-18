#pragma once

#include <fcntl.h>

#include "gtl/logging.h"

namespace gtl {

inline bool SetFdNonBlocking(int fd, bool non_blocking = true) {
  const int flags = fcntl(fd, F_GETFL, 0);
  if (flags < 0) {
    return false;
  }
  if ((flags & O_NONBLOCK) && !non_blocking) {
    return !fcntl(fd, F_SETFL, flags & ~O_NONBLOCK);
  } else if (!(flags & O_NONBLOCK) && non_blocking) {
    return !fcntl(fd, F_SETFL, flags | O_NONBLOCK);
  }
  return 0;
}

inline bool IsFdNonBlocking(int fd) {
  const int flags = fcntl(fd, F_GETFL, 0);
  if (flags < 0) {
    return false;
  }
  return flags & O_NONBLOCK;
}

}  // namespace gtl