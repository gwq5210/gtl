#pragma once

#include <pthread.h>

#include "gtl/thread/mutex.h"

namespace gtl {

class CondVar {
 public:
  CondVar() { Init(); }
  ~CondVar() { Destroy(); }

  using Handle = pthread_cond_t;

  Handle& NativeHandle() { return cond_var_; }
  const Handle& NativeHandle() const { return cond_var_; }

  bool Signal() { return !pthread_cond_signal(&cond_var_); }
  bool Broadcast() { return !pthread_cond_broadcast(&cond_var_); }
  bool Wait(Mutex& lock) { return !pthread_cond_wait(&cond_var_, &lock.NativeHandle()); }
  bool TimedWait(Mutex& lock, time_t timeout_ms) {
    if (timeout_ms >= 0) {
      struct timespec ts = MsToAbsTimeSpec(timeout_ms);
      return !pthread_cond_timedwait(&cond_var_, &lock.NativeHandle(), &ts);
    }
    return Wait(lock);
  }

 private:
  CondVar(const CondVar& other) = delete;
  CondVar& operator=(const CondVar& other) = delete;

  bool Init();
  bool Destroy();

  Handle cond_var_;
};

}  // namespace gtl