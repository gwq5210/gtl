#pragma once

#include <sys/time.h>
#include <time.h>
#include <unistd.h>

namespace gtl {

struct timespec ToAbsTimeSpec(time_t timeout_ms) {
  struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	ts.tv_nsec += timeout_ms * 1000;
  return ts;
}

}  // namespace gtl