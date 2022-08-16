#pragma once

#include <atomic>
#include <functional>
#include <vector>

#include "gtl/container/queue.h"
#include "gtl/container/vector.h"
#include "gtl/thread/cond_var.h"
#include "gtl/thread/mutex.h"
#include "gtl/thread/thread.h"

namespace gtl {

class IThreadPool {
 public:
  IThreadPool() {}
  virtual ~IThreadPool() { Stop(); }
  virtual bool Start(size_t thread_count) = 0;
  virtual bool AddTask(std::function<void(void)>&& task) = 0;
  virtual void Stop() = 0;
  virtual bool IsStop() const = 0;

 private:
  IThreadPool(const IThreadPool& other) = delete;
  IThreadPool& operator=(const IThreadPool& other) = delete;
};

class ThreadPool : public IThreadPool {
 public:
  ThreadPool() {}
  ~ThreadPool() { Stop(); }

  bool Start(size_t thread_count) override;
  bool AddTask(std::function<void(void)>&& task) override;
  void Stop() override {
    if (stop_) {
      return;
    }
    stop_ = true;
    cond_var_.Broadcast();
    for (auto& thread : threads_) {
      thread.Join();
    }
    threads_.clear();
  }
  bool IsStop() const override { return stop_; }

 private:
  ThreadPool(const ThreadPool& other) = delete;
  ThreadPool& operator=(const ThreadPool& other) = delete;

  void Run();

  std::atomic_bool stop_{false};
  Mutex mutex_;
  CondVar cond_var_;
  Vector<Thread> threads_;
  Queue<std::function<void(void)>> tasks_;
};

}  // namespace gtl