#pragma once

#include <errno.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

#include <functional>

#include "gtl/logging.h"

namespace gtl {

class Thread {
 public:
  using Id = pthread_t;
  struct Options {
    bool joinable = true;
    int stack_min_size = 0;  // 0表示使用默认属性
  };

  using Attr = pthread_attr_t;

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
  template <typename Function, typename... Args>
  explicit Thread(Function&& func, Args&&... args) {
    auto routine = std::bind(std::move(func), std::forward<Args>(args)...);
    Start([routine](void*) -> void* {
      routine();
      return nullptr;
    });
  }
  ~Thread() {
    if (valid_) {
      std::terminate();
    }
  }

  bool Detach();
  bool Join(void** retval = nullptr);
  bool Cancel();
  bool Start(std::function<void*(void*)>&& routine, void* data = nullptr, const Options* options = nullptr);

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