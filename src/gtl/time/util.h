#pragma once

#include <sys/time.h>
#include <time.h>
#include <unistd.h>

namespace gtl {

inline struct timespec MsToAbsTimeSpec(time_t timeout_ms) {
  struct timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  ts.tv_sec += timeout_ms / 1000;
  ts.tv_nsec += (timeout_ms % 1000) * 1000000;
  return ts;
}

inline struct timespec MsToTimeSpec(time_t timeout_ms) {
  struct timespec ts;
  ts.tv_sec = timeout_ms / 1000;
  ts.tv_nsec = (timeout_ms % 1000) * 1000000;
  return ts;
}

inline struct timeval MsToAbsTimeVal(time_t timeout_ms) {
  struct timeval tv;
  gettimeofday(&tv, nullptr);
  tv.tv_sec += timeout_ms / 1000;
  tv.tv_usec += (timeout_ms % 1000) * 1000;
  return tv;
}

inline struct timeval MsToTimeVal(time_t timeout_ms) {
  struct timeval tv;
  tv.tv_sec = timeout_ms / 1000;
  tv.tv_usec = (timeout_ms % 1000) * 1000;
  return tv;
}


}  // namespace gtl