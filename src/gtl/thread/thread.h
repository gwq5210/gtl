#pragma once

#include <errno.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

#include <atomic>
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

  static std::atomic_int thread_index;
  static int GetThreadIndex();
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
    detached_ = other.detached_;
    name_ = other.name_;

    other.Clear();
  }
  Thread& operator=(Thread&& other) {
    if (valid_) {
      GTL_INFO("thread valid");
      std::terminate();
    }

    valid_ = other.valid_;
    tid_ = other.tid_;
    detached_ = other.detached_;
    name_ = other.name_;

    other.Clear();
    return *this;
  }
  template <typename Function, typename... Args>
  explicit Thread(Function&& func, Args&&... args) {
    Start(std::bind(std::move(func), std::forward<Args>(args)...));
  }
  ~Thread() {
    if (valid_ && !detached_) {
      GTL_WARN("thread {} valid, but not detached", name());
    }
  }

  bool detached() const { return detached_; }
  void set_detached(bool detached) { detached_ = detached; }
  const std::string& name() const { return name_; }
  void set_name(const std::string& name) { name_ = name; }

  bool Detach();
  bool Join(void** retval = nullptr);
  bool Cancel();
  bool Start(std::function<void*(void*)>&& routine, const std::string& name = "", void* data = nullptr,
             const Options* options = nullptr);
  bool Start(std::function<void(void)>&& routine, const std::string& name = "", const Options* options = nullptr) {
    Start(
        [routine](void*) -> void* {
          routine();
          return nullptr;
        },
        name, nullptr, options);
  }
  bool Kill(int signo);

 private:
  struct RoutineArg {
    std::function<void*(void*)> routine;
    void* data = nullptr;
  };

  Thread(const Thread& other) = delete;
  Thread& operator=(const Thread& other) = delete;

  void GenerateName();

  void Clear() {
    valid_ = false;
    tid_ = {};
    detached_ = false;
    name_ = "";
  }

  bool valid_ = false;
  Id tid_{};
  bool detached_ = false;
  std::string name_;
};

}  // namespace gtl