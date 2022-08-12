#pragma once

#include <pthread.h>

#include <mutex>

#include "gtl/time/util.h"
#include "gtl/logging.h"

namespace gtl {

class Mutex {
 public:
  enum class Type : int {
    kDefault = PTHREAD_MUTEX_DEFAULT,
    kNormal = PTHREAD_MUTEX_NORMAL,
    kErrorCheck = PTHREAD_MUTEX_ERRORCHECK,
    kRecursive = PTHREAD_MUTEX_RECURSIVE,
  };
  struct Options {
    Type type = Type::kDefault;
  };

  using Attr = pthread_mutexattr_t;

  static Attr* NewAttr(const Options& options);
  static void DeleteAttr(Attr* attr);
  static bool GetType(Attr* Attr, Type& type);
  static bool SetType(Attr* attr, Type type);

  Mutex() { Init(); }
  Mutex(const Options* options) { Init(options); }
  ~Mutex() { Destroy(); }

  bool Lock() { return !pthread_mutex_lock(&lock_); }
  bool TryLock() { return !pthread_mutex_trylock(&lock_); }
  bool Unlock() { !pthread_mutex_unlock(&lock_); }

#if !defined(__APPLE__)
  bool TimedLock(time_t timeout_ms) {
    if (timeout_ms >= 0) {
      struct timespec ts = ToAbsTimeSpec(timeout_ms);
      return !pthread_mutex_timedlock(&lock_);
    }
    return Lock();
  }
#endif

 private:
  bool Init(const Options* options = nullptr);
  bool Destroy();

  pthread_mutex_t lock_;
};

class RWMutex {
 private:
  pthread_rwlock_t rwlock_;
};

}  // namespace gtl