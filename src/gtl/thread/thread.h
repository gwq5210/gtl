#pragma once

#include <errno.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

#include <functional>

#include "gtl/logging.h"
#include <thread>
namespace gtl {

class Thread {
 public:
  using Id = pthread_t;
  struct Options {
    bool joinable = true;
    int stack_min_size = 0;  // 0表示使用默认属性
  };

  using Attr = pthread_attr_t;

  static Id SelfId() { return pthread_self(); }
  static void Exit(void* retval = nullptr) { pthread_exit(retval); }
  static void* Routine(void* arg);
  static Attr* NewAttr(const Options& options);
  static void DeleteAttr(Attr* attr);
  static bool GetDetachState(Attr* attr, int& detach_state);
  static bool SetDetachState(Attr* attr, int detach_state);
  static bool GetStackMinSize(Attr* attr, size_t& stack_min_size);
  static bool SetStackMinSize(Attr* attr, size_t stack_min_size);

  Thread() = default;
  Thread(Thread&& other) {
    valid_ = other.valid_;
    tid_ = other.tid_;

    other.Clear();
  }
  Thread& operator=(Thread&& other) {
    if (valid_) {
      GTL_INFO("thread valid");
      std::terminate();
    }

    valid_ = other.valid_;
    tid_ = other.tid_;

    other.Clear();
    return *this;
  }
  template <typename Function, typename... Args>
  explicit Thread(Function&& func, Args&&... args) {
    Start(std::bind(std::move(func), std::forward<Args>(args)...));
  }
  ~Thread() {
    if (valid_) {
      GTL_INFO("thread valid");
      std::terminate();
    }
  }

  bool Detach();
  bool Join(void** retval = nullptr);
  bool Cancel();
  bool Start(std::function<void*(void*)>&& routine, void* data = nullptr, const Options* options = nullptr);
  bool Start(std::function<void(void)>&& routine, const Options* options = nullptr) {
    Start([routine](void*) -> void* {
      routine();
      return nullptr;
    });
  }
  bool Kill(int signo);

 private:
  struct RoutineArg {
    std::function<void*(void*)> routine;
    void* data = nullptr;
  };

  Thread(const Thread& other) = delete;
  Thread& operator=(const Thread& other) = delete;

  void Clear() {
    valid_ = false;
    tid_ = {};
  }

  bool valid_ = false;
  Id tid_{};
};

}  // namespace gtl