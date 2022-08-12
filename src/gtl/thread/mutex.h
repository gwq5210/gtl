#pragma once

#include <pthread.h>

#include <mutex>

#include "gtl/logging.h"
#include "gtl/time/util.h"

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

  using Handle = pthread_mutex_t;
  using Attr = pthread_mutexattr_t;

  static Attr* NewAttr(const Options& options);
  static void DeleteAttr(Attr* attr);
  static bool GetType(Attr* Attr, Type& type);
  static bool SetType(Attr* attr, Type type);

  Mutex() { Init(); }
  Mutex(const Options* options) { Init(options); }
  ~Mutex() { Destroy(); }

  Handle& NativeHandle() { return lock_; }
  const Handle& NativeHandle() const { return lock_; }

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
  Mutex(const Mutex& other) = delete;
  Mutex& operator=(const Mutex& other) = delete;
  bool Init(const Options* options = nullptr);
  bool Destroy();

  Handle lock_;
};

class RWMutex {
 public:
  using Handle = pthread_rwlock_t;

  RWMutex() { Init(); }
  ~RWMutex() { Destroy(); }

  Handle& NativeHandle() { return rwlock_; }
  const Handle& NativeHandle() const { return rwlock_; }

  bool RdLock() { return !pthread_rwlock_rdlock(&rwlock_); }
  bool WrLock() { return !pthread_rwlock_wrlock(&rwlock_); }
  bool TryRdLock() { return !pthread_rwlock_tryrdlock(&rwlock_); }
  bool TryWrLock() { return !pthread_rwlock_trywrlock(&rwlock_); }
  bool Unlock() { !pthread_rwlock_unlock(&rwlock_); }

#if !defined(__APPLE__)
  bool TimedRdLock(time_t timeout_ms) {
    if (timeout_ms >= 0) {
      struct timespec ts = ToAbsTimeSpec(timeout_ms);
      return !pthread_rwlock_timedrdlock(&rwlock_);
    }
    return RdLock();
  }
  bool TimedWrLock(time_t timeout_ms) {
    if (timeout_ms >= 0) {
      struct timespec ts = ToAbsTimeSpec(timeout_ms);
      return !pthread_rwlock_timedwrlock(&rwlock_);
    }
    return WrLock();
  }
#endif

 private:
  RWMutex(const RWMutex& other) = delete;
  RWMutex& operator=(const RWMutex& other) = delete;
  bool Init();
  bool Destroy();

  Handle rwlock_;
};

}  // namespace gtl