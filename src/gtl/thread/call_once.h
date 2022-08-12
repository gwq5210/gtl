#pragma once

#include <mutex>

namespace gtl {

class OnceFlag {
 public:
  using Handle = pthread_once_t;

  OnceFlag() = default;

  Handle& NativeHandle() { return once_; }
  const Handle& NativeHandle() const { return once_; }

 private:
  OnceFlag(const OnceFlag& other) = delete;
  OnceFlag& operator=(const OnceFlag& other) = delete;

  Handle once_ = PTHREAD_ONCE_INIT;
};

using OnceRoutine = void (*)();

bool CallOnce(OnceFlag& once_flag, OnceRoutine once_routine) {
  return !pthread_once(&once_flag.NativeHandle(), once_routine);
}

}  // namespace gtl