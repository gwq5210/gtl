#pragma once

#include <vector>
#include <atomic>
#include <functional>

#include "gtl/container/queue.h"
#include "gtl/container/vector.h"
#include "gtl/thread/cond_var.h"
#include "gtl/thread/mutex.h"
#include "gtl/thread/thread.h"

namespace gtl {

class ThreadPool {
 public:
  explicit ThreadPool(size_t thread_count, const std::string& name = "ThreadPool") { Start(thread_count, name); }
  ~ThreadPool() { Stop(); }

  const std::string& name() const { return name_; }
  void set_name(const std::string& name) { name_ = name; }

  bool Start(size_t thread_count, const std::string& name = "ThreadPool");
  bool AddTask(std::function<void(void)>&& task);
  bool Stop() {
    stop_ = true;
    cond_var_.Broadcast();
    for (auto& thread : threads_) {
      thread.Join();
    }
    threads_.clear();
  }

 private:
  ThreadPool(const ThreadPool& other) = delete;
  ThreadPool& operator=(const ThreadPool& other) = delete;

  void Routine();

  std::string name_ = "ThreadPool";
  std::atomic_bool stop_{false};
  Mutex mutex_;
  CondVar cond_var_;
  Vector<Thread> threads_;
  Queue<std::function<void(void)>> tasks_;
};

}  // namespace gtl